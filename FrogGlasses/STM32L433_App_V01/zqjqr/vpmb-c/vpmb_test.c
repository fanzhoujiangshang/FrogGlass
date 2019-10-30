#include "vpmb.h"

/* Copyright 2010, Bryan Waite, Erik C. Baker. All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Bryan Waite, Erik C. Baker ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Bryan Waite, or Erik C. Baker OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Bryan Waite, or Erik C. Baker.
 *
 */

/** \mainpage VPM-B Dive Algorithm
 *
 * \section intro_sec Introduction
 *
 */

int update_depth(dive_state *dive, double increment_time, double rate, dive_profile *current_profile)
{
    /* Currently this is just part of a driver to test the real time dive loop
     *  this would be changed to just read the current depth from a sensor and then update the rate
     * in a real dive computer
     */
    dive->Last_Depth = dive->Depth;

    /* ascent or descent */
    if(current_profile->profile_code == 1) {
        /* descending */
        if(rate > 0 && dive->Depth < current_profile->ending_depth) {
            dive->Depth = dive->Depth + (rate * increment_time);
            /* if (dive->Depth > current_profile->ending_depth) */
            /*         dive->Depth = current_profile->ending_depth; */
        } /* ascending */
        else if (rate < 0 && dive->Depth > current_profile->ending_depth) {
            /* printf("ASCENTS FROM %f", dive->Depth); */
            dive->Depth = dive->Depth + (rate * increment_time);
        } else {
            return DONE_PROFILE;
        }

    } else if(current_profile->profile_code == 99) {
        if(dive->decomp_stops != NULL) {
            printf("stop: %d, depth: %f, time: %f\n", dive->decomp_stop_index, dive->Depth, dive->Run_Time);


            if(dive->decomp_stops[dive->decomp_stop_index].ascent_or_const == ASCENT) {
                dive->Depth = dive->Depth + (rate * increment_time);

                /* adjust if we go over */
                if(dive->Depth < dive->decomp_stops[dive->decomp_stop_index].depth) {
                    dive->Depth = dive->decomp_stops[dive->decomp_stop_index].depth;
                }

                /* reached the depth */
                if(dive->Depth == dive->decomp_stops[dive->decomp_stop_index].depth) {
                    dive->decomp_stop_index++;
                }

            } else {
                if(dive->Run_Time >= dive->Wait_Time) {
                    dive->decomp_stop_index++;
                } else {
                    dive->Run_Time += increment_time;
                }
            }
        }
    }
		//printf("DEPTH_CHANGED\r\n");
		usr_delay_ms(1);
    return DEPTH_CHANGED;
}

void calc_current_deco_zone(dive_state * dive, single_dive *current_dive)
{
    dive_profile *current_profile = NULL;
    int j;
    double decomp_zone;
    for(j = 0; j < current_dive->num_profile_codes; j++) {
        current_profile = &(current_dive->dive_profiles[j]);
        if (current_profile->profile_code == 99) {
            break;
        }
    }
    decomp_zone = vpmb_find_start_of_decompression_zone(dive, current_profile);
    printf("Deepest possible decompression stop is %f\n", decomp_zone);
    dive->Start_of_Decompression_Zone = decomp_zone;
}

void real_time_dive(dive_state *dive, json_input *input)
{
    int i, j;
	int cmd;

    single_dive *current_dive;
    dive->Real_Time_Decompression = TRUE;
    for(i = 0; i < input->number_of_dives; i++) {
        dive_profile *current_profile;
        BOOL dive_finished = FALSE;
        BOOL calc_decompression_stop = FALSE;
        direction cur_dir;
        const double increment_time = 0.5; /* minutes */

        current_dive = &(input->dives[i]);
        vpmb_set_gas_mixes(dive, current_dive);

        current_profile = &(current_dive->dive_profiles[0]);
        dive->Starting_Depth = current_profile->starting_depth;
        dive->Ending_Depth = current_profile->ending_depth;
        dive->Rate = current_profile->rate;
        dive->Mix_Number = current_profile->gasmix;
        for(j = 0; j < current_dive->num_profile_codes; j++) {
            dive_profile *current_profile = &(current_dive->dive_profiles[j]);
            double current_rate = current_profile->rate;
            /* printf("current_rate: %f",current_rate); */
            if(current_profile->profile_code == 99) {
                dive->Decompressing = TRUE;
                current_rate = current_profile->ascents[0].rate;
            }
            while(!dive_finished) {
                /* change direction or profiles */
                if(update_depth(dive, increment_time, current_rate, current_profile) == DONE_PROFILE) {
			printf("DONE_PROFILE\n");
                    break;
                }

                /* double rate; */
                /* dive->Rate = current_profile->rate; */
                if(dive->Decompressing == FALSE) {
                    cur_dir = vpmb_current_direction(dive, increment_time);
                    /* int check = msw_driver(dive); */

                    /* if (check == 1 || check ==2) { */

                    /* regular ascent, or ascent to start of decompression zone */
                    if(cur_dir == ASCENT) {
                        /* else{ */
                        vpmb_gas_loadings_ascent_descent(dive, dive->Last_Depth, dive->Depth, dive->Rate);
                        printf("Ascending: Depth %f, Run_Time: %f\n", dive->Depth, dive->Run_Time);
                        continue;
                        /* } */
                    } else if(cur_dir == DESCENT) {
                        vpmb_gas_loadings_ascent_descent(dive, dive->Last_Depth, dive->Depth, dive->Rate);

                        vpmb_calc_crushing_pressure(dive, dive->Last_Depth, dive->Depth, dive->Rate);
                        printf("Descending: Depth %f, Run_Time: %f\n", dive->Depth, dive->Run_Time);
                    } else if (cur_dir == CONSTANT) {
                        /* else{ */
                        if(vpmb_finished_constant_depth_profile(dive, current_profile) == DONE_PROFILE) {
                            break;
                        }

                        vpmb_gas_loadings_constant_depth(dive, dive->Depth, dive->Run_Time + increment_time);
                        printf("Constant: Depth: %f, Run_Time: %f\n", dive->Depth, dive->Run_Time);
                        /* } */
                    }
                } else {
			//printf("�?始减压\n");
                    if(dive->decomp_stops == NULL) {
                        double time = dive->Run_Time;
                        /* double depth = dive->Depth; */
                        vpmb_calculate_decompression_stops(dive, current_profile);
                        vpmb_decompress_init(dive, current_profile);
                        dive->Run_Time = time;
                        /* dive->Depth = depth; */
                    } else {
                        if(dive->Depth <= 0) {
                            dive_finished = TRUE;
                        } else if (dive->decomp_stops[dive->decomp_stop_index].ascent_or_const == ASCENT) {
                            printf("ASCENT\t");
                            vpmb_gas_loadings_ascent_descent(dive, dive->Last_Depth, dive->Depth, dive->Rate);
                            vpmb_calc_max_actual_gradient(dive, dive->Deco_Stop_Depth);
                            calc_decompression_stop = TRUE;
                        } else {
                            printf("CONST\t");
                            if(calc_decompression_stop == TRUE) {
                                double run_time = dive->Run_Time;
                                vpmb_critical_volume_decision_tree_to_depth(dive, dive->decomp_stops[dive->decomp_stop_index].depth - 0.1);
                                dive->Wait_Time = dive->Run_Time;
                                dive->Run_Time = run_time;
                                calc_decompression_stop = FALSE;
                            }
                            /* dive->decomp_stop_index++; */
                        }

                    }
                    /* double zone_upper_bound = dive->Start_of_Decompression_Zone - 1; */
                    /* if(dive->Depth <= dive->Start_of_Decompression_Zone && dive->Depth >= zone_upper_bound){ */

                    /* printf("Waiting to decompress"); */
                    /* vpmb_gas_loadings_ascent_descent(dive, dive->Starting_Depth, dive->Depth_Start_of_Deco_Zone, dive->Rate); */
                    /* dive->Run_Time_Start_of_Deco_Zone = dive->Run_Time; */
                    /* dive->Deco_Phase_Volume_Time = 0.0; */
                    /* dive->Last_Run_Time = 0.0; */
                    /* dive->Schedule_Converged = FALSE; */

                    /* for(int k=0; k < 16; k++){ */
                    /*         dive->Last_Phase_Volume_Time[k] = 0.0; */
                    /*         dive->He_Pressure_Start_of_Deco_Zone[k] = dive->Helium_Pressure[k]; */
                    /*         dive->N2_Pressure_Start_of_Deco_Zone[k] = dive->Nitrogen_Pressure[k]; */
                    /*         dive->Max_Actual_Gradient[k] = 0.0; */
                    /* } */
                    /* CALC_ASCENT_CEILING(dive); */
                    /* } */

                }
                /* calc_current_deco_zone(dive, current_dive); */
                /* } */
                /* decompress */
                /* else if(check == 99){ */
                /* printf("Ascending: Depth %f\n", dive->Depth); */
                /* current_rate = -10; */
                /* dive->Rate = -10; */
                /* if (dive->Depth > dive->Start_of_Decompression_Zone) */
                /*         vpmb_gas_loadings_ascent_descent(dive, dive->Last_Depth, dive->Depth, dive->Rate); */
                /* } */
            }
        }
    }
}


void output_array_double(char *name, double arr[], int len)
{
    int i;
    printf("%s: [", name);
    for(i = 0; i < len; i++) {
        printf("%f, ", arr[i]);
    }
    printf("]\n");
}

void output_array_int(char *name, int arr[], int len)
{
    int i;
    printf("%s: [", name);
    for(i = 0; i < len; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void output_dive_state(dive_state *dive)
{

    printf("Units: %s\n", dive->Units);
    printf("Units_Word1: %s\n", dive->Units_Word1);
    printf("Units_Word2: %s\n", dive->Units_Word2);

    printf("Water_Vapor_Pressure: %f \n", dive->Water_Vapor_Pressure);
    printf("Surface_Tension_Gamma: %f\n", dive->Surface_Tension_Gamma);
    printf("Skin_Compression_GammaC: %f\n", dive->Skin_Compression_GammaC);
    printf("Crit_Volume_Parameter_Lambda: %f\n", dive->Crit_Volume_Parameter_Lambda);
    printf("Minimum_Deco_Stop_Time: %f\n", dive->Minimum_Deco_Stop_Time);
    printf("Regeneration_Time_Constant: %f\n", dive->Regeneration_Time_Constant);
    printf("Constant_Pressure_Other_Gases: %f\n", dive->Constant_Pressure_Other_Gases);
    printf("Gradient_Onset_of_Imperm_Atm: %f\n", dive->Gradient_Onset_of_Imperm_Atm);

    printf("Number_of_Changes: %d\n", dive->Number_of_Changes);
    printf("Segment_Number_Start_of_Ascent: %d\n", dive->Segment_Number_Start_of_Ascent);
    printf("Repetitive_Dive_Flag: %d\n", dive->Repetitive_Dive_Flag);
    printf("Schedule_Converged: %d\n", dive->Schedule_Converged);
    printf("Critical_Volume_Algorithm_Off: %d\n", dive->Critical_Volume_Algorithm_Off);
    printf("Altitude_Dive_Algorithm_Off: %d\n", dive->Altitude_Dive_Algorithm_Off);

    printf("Ascent_Ceiling_Depth: %f\n", dive->Ascent_Ceiling_Depth);
    printf("Deco_Stop_Depth: %f\n", dive->Deco_Stop_Depth);
    printf("Step_Size: %f\n", dive->Step_Size);
    printf("Depth: %f\n", dive->Depth);
    printf("Last_Depth: %f\n", dive->Last_Depth);
    printf("Ending_Depth: %f\n", dive->Ending_Depth);
    printf("Starting_Depth: %f\n", dive->Starting_Depth);
    printf("Rate: %f\n", dive->Rate);
    printf("Run_Time_End_of_Segment: %f\n", dive->Run_Time_End_of_Segment);
    printf("Last_Run_Time: %f\n", dive->Last_Run_Time);
    printf("Stop_Time: %f\n", dive->Stop_Time);
    printf("Depth_Start_of_Deco_Zone: %f\n", dive->Depth_Start_of_Deco_Zone);
    printf("Deepest_Possible_Stop_Depth: %f\n", dive->Deepest_Possible_Stop_Depth);
    printf("First_Stop_Depth: %f\n", dive->First_Stop_Depth);
    printf("Critical_Volume_Comparison: %f\n", dive->Critical_Volume_Comparison);
    printf("Next_Stop: %f\n", dive->Next_Stop);
    printf("Run_Time_Start_of_Deco_Zone: %f\n", dive->Run_Time_Start_of_Deco_Zone);
    printf("Critical_Radius_N2_Microns: %f\n", dive->Critical_Radius_N2_Microns);
    printf("Critical_Radius_He_Microns: %f\n", dive->Critical_Radius_He_Microns);
    printf("Run_Time_Start_of_Ascent: %f\n", dive->Run_Time_Start_of_Ascent);
    printf("Altitude_of_Dive: %f\n", dive->Altitude_of_Dive);
    printf("Deco_Phase_Volume_Time: %f\n", dive->Deco_Phase_Volume_Time);
    printf("Surface_Interval_Time: %f\n", dive->Surface_Interval_Time);

    output_array_int("Mix_Change", dive->Mix_Change, 16);
    output_array_int("Depth_Change", dive->Depth_Change, 16);
    output_array_int("Rate_Change", dive->Rate_Change, 16);
    output_array_int("Step_Size_Change", dive->Step_Size_Change, 16);

    output_array_double("Regenerated_Radius_He", dive->Regenerated_Radius_He, 16);
    output_array_double("Regenerated_Radius_N2", dive->Regenerated_Radius_N2, 16);
    output_array_double("He_Pressure_Start_of_Ascent", dive->He_Pressure_Start_of_Ascent, 16);
    output_array_double("N2_Pressure_Start_of_Ascent", dive->N2_Pressure_Start_of_Ascent, 16);
    output_array_double("He_Pressure_Start_of_Deco_Zone", dive->He_Pressure_Start_of_Deco_Zone, 16);
    output_array_double("N2_Pressure_Start_of_Deco_Zone", dive->N2_Pressure_Start_of_Deco_Zone, 16);
    output_array_double("Phase_Volume_Time", dive->Phase_Volume_Time, 16);
    output_array_double("Last_Phase_Volume_Time", dive->Last_Phase_Volume_Time, 16);
    output_array_double("Allowable_Gradient_He", dive->Allowable_Gradient_He, 16);
    output_array_double("Allowable_Gradient_N2", dive->Allowable_Gradient_N2, 16);
    output_array_double("Adjusted_Crushing_Pressure_He", dive->Adjusted_Crushing_Pressure_He, 16);
    output_array_double("Adjusted_Crushing_Pressure_N2", dive->Adjusted_Crushing_Pressure_N2, 16);
    output_array_double("Initial_Allowable_Gradient_N2", dive->Initial_Allowable_Gradient_N2, 16);
    output_array_double("Initial_Allowable_Gradient_He", dive->Initial_Allowable_Gradient_He, 16);
    output_array_double("Deco_Gradient_He", dive->Deco_Gradient_He, 16);
    output_array_double("Deco_Gradient_N2", dive->Deco_Gradient_N2, 16);

    printf("Segment_Number: %d\n", dive->Segment_Number);
    printf("Mix_Number: %d\n", dive->Mix_Number);
    printf("units_fsw: %d\n", dive->units_fsw);

    printf("Run_Time: %f\n", dive->Run_Time);
    printf("Segment_Time: %f\n", dive->Segment_Time);
    printf("Ending_Ambient_Pressure: %f\n", dive->Ending_Ambient_Pressure);
    printf("Barometric_Pressure: %f\n", dive->Barometric_Pressure);
    printf("Units_Factor: %f\n", dive->Units_Factor);

    output_array_double("Helium_Time_Constant", dive->Helium_Time_Constant, 16);
    output_array_double("Nitrogen_Time_Constant", dive->Nitrogen_Time_Constant, 16);
    output_array_double("Helium_Pressure", dive->Helium_Pressure, 16);
    output_array_double("Nitrogen_Pressure", dive->Nitrogen_Pressure, 16);
    output_array_double("Initial_Helium_Pressure", dive->Initial_Helium_Pressure, 16);
    output_array_double("Initial_Nitrogen_Pressure", dive->Initial_Nitrogen_Pressure, 16);
    output_array_double("Initial_Critical_Radius_He", dive->Initial_Critical_Radius_He, 16);
    output_array_double("Initial_Critical_Radius_N2", dive->Initial_Critical_Radius_N2, 16);
    output_array_double("Adjusted_Critical_Radius_He", dive->Adjusted_Critical_Radius_He, 16);
    output_array_double("Adjusted_Critical_Radius_N2", dive->Adjusted_Critical_Radius_N2, 16);
    output_array_double("Max_Crushing_Pressure_He", dive->Max_Crushing_Pressure_He, 16);
    output_array_double("Max_Crushing_Pressure_N2", dive->Max_Crushing_Pressure_N2, 16);
    output_array_double("Surface_Phase_Volume_Time", dive->Surface_Phase_Volume_Time, 16);
    output_array_double("Max_Actual_Gradient", dive->Max_Actual_Gradient, 16);
    output_array_double("Amb_Pressure_Onset_of_Imperm", dive->Amb_Pressure_Onset_of_Imperm, 16);
    output_array_double("Gas_Tension_Onset_of_Imperm", dive->Gas_Tension_Onset_of_Imperm, 16);

    /* double *Fraction_Helium; */
    /* double *Fraction_Nitrogen; */

    printf("Diver_Acclimatized: %d\n", dive->Diver_Acclimatized);
    printf("Decompressing: %d\n", dive->Decompressing);
    printf("Real_Time_Decompression: %d\n", dive->Real_Time_Decompression);

    printf("Last_Direction_Depth: %f\n", dive->Last_Direction_Depth);
    printf("Last_Direction_Time: %f\n", dive->Last_Direction_Time);

    printf("Start_of_Decompression_Zone: %f\n", dive->Start_of_Decompression_Zone);

    /* decompression_stops *decomp_stops; */
    printf("decomp_stop_index: %d\n", dive->decomp_stop_index);
    printf("Wait_Time: %f\n", dive->Wait_Time);

}

int vpmb_test(vpmb_input_data *data, int cmd)
{
    dive_state *test_dive;
    json_input input;

   // if(argc < 2) {
     //   printf("Must provide a path to an input file\n");
       // return -1;
//    }

    if (vpmb_load_from_json(&input, data) == BADJSON) {
        return -1;
    }

    test_dive = malloc(sizeof(dive_state));

    if (vpmb_validate_data(&input, test_dive) == INVALIDDATA) {
        printf("1.INVALID DATA IN INPUT FILE. PLEASE CHECK IT AND RUN THIS AGAIN\n");
        return BADJSON;
    }
    if (vpmb_initialize_data(&input, test_dive) == INVALIDDATA) {
        printf("2.INVALID DATA IN INPUT FILE. PLEASE CHECK IT AND RUN THIS AGAIN\n");
        return BADJSON;
    }

    /* default is real time, but passing in 1 allows for ahead of time profile */
    if (1 == cmd) {
				vpmb_repetitive_dive_loop(test_dive, &input);
    } else {
        real_time_dive(test_dive, &input);
    }

    output_dive_state(test_dive);

    vpmb_free_dives(&input);
    vpmb_free_dive_state(test_dive);

    return 0;
}

