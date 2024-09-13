#include <stdint.h>
#include <stdlib.h>
// https://en.wikichip.org/wiki/WikiChip

/*
Coffe Lake: https://en.wikichip.org/wiki/intel/microarchitectures/coffee_lake
*/

/*Alder Lake S CPU's(https://en.wikichip.org/wiki/intel/cores/alder_lake_s)
Model	Launched	Price	Family	Cores	Threads	L3$	TDP	Frequency	Turbo	Max Mem	Name	Frequency	Turbo
i5-12600K	4 November 2021	$ 289.00, $ 299.00	Core i5	10	16	2 MiB, 18 MiB	125 W	3.7 GHz		128 GiB	UHD Graphics 770	300 MHz	1,450 MHz
i5-12600KF	4 November 2021	$ 264.00, $ 274.00	Core i5	10	16	2 MiB, 18 MiB	125 W	3.7 GHz		128 GiB			
i7-12700K	4 November 2021	$ 409.00, $ 419.00	Core i7	12	20	1 MiB, 24 MiB	125 W	3.6 GHz		128 GiB	UHD Graphics 770	300 MHz	1,500 MHz
i7-12700KF	4 November 2021	$ 384.00, $ 394.00	Core i7	12	20	1 MiB, 24 MiB	125 W	3.6 GHz		128 GiB			
i9-12900K	4 November 2021	$ 589.00, $ 599.00	Core i9	16	24	6 MiB, 24 MiB	125 W	3.2 GHz		128 GiB	UHD Graphics 770	300 MHz	1,550 MHz
i9-12900KF	4 November 2021	$ 564.00, $ 574.00	Core i9	16	24	6 MiB, 24 MiB	125 W	3.2 GHz		128 GiB	

https://en.wikichip.org/wiki/intel/microarchitectures/alder_lake

Die
Name	CPU Configuration	                    GPU	Dimensions	Area
ADL-S	8P + 8E	                                 32 EU	        10.5 mm x 20.5 mm	215.25 mm²
        6P + 0E	                                                10.5 mm x 15.5 mm	162.75 mm²
ADL-P	6P + 8E	                                 96 EU
ADL-M	2P + 8E

Core	Extended Family	 Family	    Extended Model	    Model
S	            0	        0x6	    0x9	                0x7
                        Family 6 Model 151
P	            0	        0x6	    0x9	                0xA
                        Family 6 Model 154


Family  	General Description	            Differentiating Features
                                            Cores	    HT	AVX	AVX2 TBT TBMT
Core i3	    Low-end Performance	            4  (4+0)    ✔	✔	✔	✔	✘
Core i5	    Mid-range Performance	        10 (6+4) 	✔	✔	✔	✔	✘
                                            6  (6+0)
Core i7	    High-end Performance	        12 (8+4)	✔	✔	✔	✔	✔
Core i9	    Extreme Performance	            16 (8+8)	✔	✔	✔	✔	✔

*/

// Intel cpuid: https://en.wikichip.org/wiki/intel/cpuid#:~:text=Below%20is%20a%20list%20of%20Intel's
// AMD cpuid:   https://en.wikichip.org/wiki/amd/cpuid
#define get_model_number(Family_ID, Model_ID, Extended_Model_ID) ((Family_ID == 0x06) || (Family_ID == 0x0f)) ? ((Extended_Model_ID << 4) + Model_ID) : Model_ID

#define _CLIENT_arch(name) name ## _c
#define _SERVER_arch(name) name ## _s
#define CLIENT_arch(name) _CLIENT_arch(name)
#define SERVER_arch(name) _SERVER_arch(name)

#define string_arch(name) #name

typedef enum micro_arch {
    // Family 15
    Netburst,

    // Family 11
    Knights_Ferry,
    Knights_Corner,

    // Big Cores (Client) (Family 6)
    Meteor_Lake,
    Raptor_Lake,
    Alder_Lake,
    Rocket_Lake,
    Tiger_Lake,
    CLIENT_arch(Ice_Lake),
    Comet_Lake,
    Amber_Lake,
    Whiskey_Lake,
    Cannon_Lake,
    Coffee_Lake,
    Kaby_Lake,
    CLIENT_arch(Skylake),
    CLIENT_arch(Broadwell),
    CLIENT_arch(Haswell),
    CLIENT_arch(Sandy_Bridge),
    CLIENT_arch(Westmere),
    CLIENT_arch(Nehalem),
    CLIENT_arch(Penryn),
    CLIENT_arch(Core),
    Modified_Pentium_M,
    Pentium_M,
    CLIENT_arch(P6),

    // Big Cores (Server) (Family 6)
    Diamond_Rapids,
    Granite_Rapids,
    Emerald_Rapids,
    Sapphire_Rapids,
    SERVER_arch(Ice_Lake),
    Cooper_Lake,
    Cascade_Lake,
    SERVER_arch(Skylake),
    SERVER_arch(Broadwell),
    SERVER_arch(Haswell),
    SERVER_arch(Sandy_Bridge),
    SERVER_arch(Westmere),
    SERVER_arch(Nehalem),
    SERVER_arch(Penryn),
    SERVER_arch(P6),

    // Small Cores (Family 6)
    Tremont,
    Goldmont_Plus,
    Goldmont,
    Airmont,
    Silvermont,
    Saltwell,
    Bonnell,

    // MIC Architecture (Family 6)
    Knights_Mill,
    Knights_Landing,

    // Family 5
    Lakemont,
    P5,

    // Family 4
    i80486,

    // Family 3
    i80386
} micro_arch;

typedef struct cpu_model {
    uint32_t arch;
    uint8_t core;

    uint8_t  extended_model:4;
    uint8_t           model:4;

    uint8_t extended_family:4;
    uint8_t          family:4;
} cpu_model;

static const cpu_model cpu_models[] = {
    /*
     * Family 15
     * Microarchitecture	Core	Extended Family	    Family	Extended Model	Model
     * Netburst		                     0	            0xF 	0x0	            0x6	    Family 15 Model 6
     *                  Prescott         0	            0xF 	0x0 	        0x4	    Family 15 Model 4
     *                  Prescott	     0	            0xF 	0x0 	        0x3	    Family 15 Model 3
     *                  Northwood	     0	            0xF 	0x0	            0x2	    Family 15 Model 2
     *                  Willamette	     0	            0xF 	0x0	            0x1	    Family 15 Model 1
     * 
    */
    // Family 15
    {   // Netburst
        .arch = Netburst,
        .core = 0, // Prescott

        .extended_model  = 0x0,
        .model           = 0x6,

        .extended_family = 0x0,
        .family          = 0xf
        // 	Family 15 Model 6
    }, { // Netburst
        .arch = Netburst,
        .core = 0, // Prescott

        .extended_model  = 0x0,
        .model           = 0x4,

        .extended_family = 0x0,
        .family          = 0xf
        // Family 15 Model 4
    }, { // Netburst
        .arch = Netburst,
        .core = 0, // Northwood	

        .extended_model  = 0x0,
        .model           = 0x3,

        .extended_family = 0x0,
        .family          = 0xf
        // Family 15 Model 3
    }, { // Netburst
        .arch = Netburst,
        .core = 0, // Willamette	

        .extended_model  = 0x0,
        .model           = 0x2,

        .extended_family = 0x0,
        .family          = 0xf
        // Family 15 Model 2
    }, { // Netburst
        .arch = Netburst,
        .core = 0,

        .extended_model  = 0x0,
        .model           = 0x1,

        .extended_family = 0x0,
        .family          = 0xf
        // 	Family 15 Model 1
    }, 
    
    /*
    * Family 11
    * Microarchitecture	Core	Extended Family	    Family	    Extended Model	Model
    * Knights Ferry		            0	            0xB	            0x0	        0x0	    Family 11 Model 0
    * Knights Corner		        0	            0xB	            0x0	        0x1	    Family 11 Model 1
    */
    // Family 11
    {   // Knights Ferry
        .arch = Knights_Ferry,
        .core = 0, 

        .extended_model  = 0x0,
        .model           = 0x0,

        .extended_family = 0x0,
        .family          = 0xb
        // 	Family 11 Model 0
    }, { // Knights Corner
        .arch = Knights_Corner,
        .core = 0, 

        .extended_model  = 0x0,
        .model           = 0x1,

        .extended_family = 0x0,
        .family          = 0xb
        // 	Family 11 Model 1
    },

/*
Microarchitecture	    Core	                    Extended Family	    Family	Extended Model	    Model
Meteor Lake 
Raptor Lake	             S	                            0	             0x6	    0xB	             0x7	    Family 6 Model 183
                         P	                            0	             0x6	    0xB	             0xA	    Family 6 Model 186
Alder Lake	             S	                            0	             0x6	    0x9	             0x7	    Family 6 Model 151
                         P	                            0	             0x6	    0x9	             0xA	    Family 6 Model 154
Rocket Lake              S	                            0	             0x6	    0xA	             0x7	    Family 6 Model 167
Tiger Lake	             H	                            0	             0x6	    0x8	             0xD	    Family 6 Model 141
                         U	                            0	             0x6	    0x8	             0xC	    Family 6 Model 140
Ice Lake (Client)	     U	                            0	             0x6	    0x7	             0xE	    Family 6 Model 126
                         Y	                            0	             0x6	    0x7	             0xE	    Family 6 Model 126
Comet Lake	             S, H	                        0                0x6	    0xA	             0x5	    Family 6 Model 165
                         U	                            0	             0x6	    0x8	             0xE	    Family 6 Model 142
Amber Lake	             Y	                            0	             0x6	    0x6	             0x6	    Family 6 Model 102
Whiskey Lake	         U	                            0	             0x6	    0x6	             0x6	    Family 6 Model 102
Cannon Lake	             U	                            0	             0x6	    0x6	             0x6	    Family 6 Model 102
Coffee Lake	             S,H,E	                        0	             0x6	    0x9	             0xE	    Family 6 Model 158
                         U	                            0	             0x6	    0x8	             0xE	    Family 6 Model 142
Kaby Lake	             DT, H, S, X	                0	             0x6	    0x9	             0xE	    Family 6 Model 158
                         Y, U       	                0        	     0x6    	0x8          	 0xE    	Family 6 Model 142
Skylake (Client)	     DT, H, S	                    0	             0x6	    0x5          	 0xE	    Family 6 Model 94
                         Y, U	                        0	             0x6	    0x4          	 0xE	    Family 6 Model 78
Broadwell (Client)	     C, W	                        0	             0x6	    0x4          	 0x7	    Family 6 Model 71
                         U, Y, S	                    0	             0x6	    0x3          	 0xD	    Family 6 Model 61
                         H	                            0	             0x6	    0x4          	 0x7	    Family 6 Model 71
Haswell (Client)	     GT3E	                        0	             0x6	    0x4          	 0x6	    Family 6 Model 70
                         ULT	                        0	             0x6	    0x4          	 0x5	    Family 6 Model 69
                         S	                            0	             0x6	    0x3          	 0xC	    Family 6 Model 60
Ivy Bridge (Client)	     M, H, Gladden	                0	             0x6	    0x3          	 0xA	    Family 6 Model 58
Sandy Bridge (Client)	 M, H, Celeron	                0	             0x6	    0x2          	 0xA	    Family 6 Model 42
Westmere (Client)	     Arrandale, Clarkdale     	    0	             0x6    	0x2          	 0x5     	Family 6 Model 37
Nehalem (Client)	     Auburndale, Havendale    	    0	             0x6    	0x1          	 0xF     	Family 6 Model 31
                         Clarksfield	                0	             0x6	    0x1	             0xE    	Family 6 Model 30
Penryn (Client)	         Penryn, Wolfdale, Yorkfield	0	             0x6	    0x1	             0x7	    Family 6 Model 23
Core (Client)	         Merom L	                    0	             0x6	    0x1	             0x6	    Family 6 Model 22
                         Merom	                        0	             0x6	    0x0	             0xF	    Family 6 Model 15
Modified Pentium M	     Yonah	                        0	             0x6	    0x0	             0xE	    Family 6 Model 14
Pentium M	             Tolapai	                    0	             0x6	    0x1	             0x5	    Family 6 Model 21
                         Dothan	                        0	             0x6	    0x0	             0xD	    Family 6 Model 13
                         Banias	                        0	             0x6	    0x0	             0x9	    Family 6 Model 9
P6	                     Tualatin	                    0	             0x6	    0x0	             0xB	    Family 6 Model 11
                         Coppermine, Coppermine T	    0	             0x6	    0x0	             0x8	    Family 6 Model 8
                         Katmai	                        0	             0x6	    0x0	             0x7	    Family 6 Model 7
                                                        0	             0x6	    0x0	             0x6	    Family 6 Model 6
                                                        0	             0x6	    0x0	             0x5	    Family 6 Model 5
                                                        0	             0x6	    0x0	             0x3	    Family 6 Model 3
                                                        0	             0x6	    0x0	             0x1	    Family 6 Model 1
 */
    // Family 6
    // ¿Meteor Lake?
    //
    {   // Raptor Lake
        .arch = Raptor_Lake,
        .core = 0,  // S

        .extended_model  = 0xb,
        .model           = 0x7,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 183
    }, { // Raptor Lake
        .arch = Raptor_Lake,
        .core = 0,  // P

        .extended_model  = 0xb,
        .model           = 0xa,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 186
    },

    {   // Alder Lake
        .arch = Alder_Lake,
        .core = 0,  // S

        .extended_model  = 0x9,
        .model           = 0x7,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 151
    }, { // Alder Lake
        .arch = Alder_Lake,
        .core = 0,  // P

        .extended_model  = 0x9,
        .model           = 0xa,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 154
    }, 
    
    { // Rocket Lake
        .arch = Rocket_Lake,
        .core = 0,  // S

        .extended_model  = 0xa,
        .model           = 0x7,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 167
    },

    { // Tiger Lake
        .arch = Tiger_Lake,
        .core = 0,  // H

        .extended_model  = 0x8,
        .model           = 0xc,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 141
    }, { // Tiger Lake
        .arch = Tiger_Lake,
        .core = 0,  // U

        .extended_model  = 0x8,
        .model           = 0xc,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 140
    },

    { // Ice Lake (Client)
        .arch = CLIENT_arch(Ice_Lake),
        .core = 0,  // U

        .extended_model  = 0x7,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 126
    }, { // Ice Lake (Client)
        .arch = CLIENT_arch(Ice_Lake),
        .core = 0,  // Y

        .extended_model  = 0x7,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 126
    },

    {   // Comet Lake
        .arch = Comet_Lake,
        .core = 0,  // S, H

        .extended_model  = 0xa,
        .model           = 0x5,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 165
    },
    
    { // Comet Lake && Amber Lake && Whiskey Lake
        .arch = Comet_Lake << 16 | Amber_Lake << 8 | Whiskey_Lake , 
        .core = 0,  /*
                     * IF (Comet Lake)   THEN Core = U
                     * IF (Amber Lake)   THEN Core = Y
                     * IF (Whiskey Lake) THEN Core = U
                     */

        .extended_model  = 0x8,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 		Family 6 Model 142
    },
    
    {   // Cannon Lake
        .arch = Cannon_Lake,
        .core = 0,  // U

        .extended_model  = 0x6,
        .model           = 0x6,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 102
    },

    {   // Coffee Lake
        .arch = Coffee_Lake,
        .core = 0,  // 	S, H, E

        .extended_model  = 0x9,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 158
    }, {   // Coffee Lake
        .arch = Coffee_Lake,
        .core = 0,  // 	U

        .extended_model  = 0x8,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 142
    },

    {   // Kaby Lake
        .arch = Kaby_Lake,
        .core = 0,  // DT, H, S, X

        .extended_model  = 0x9,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 158
    }, {   // Kaby Lake
        .arch = Kaby_Lake,
        .core = 0,  // 	Y, U

        .extended_model  = 0x8,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 142
    },

    {   // Skylake (Client)
        .arch = CLIENT_arch(Skylake),
        .core = 0,  // DT, H, S

        .extended_model  = 0x5,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 94
    }, {   // Skylake (Client)
        .arch = CLIENT_arch(Skylake),
        .core = 0,  // Y, U

        .extended_model  = 0x4,
        .model           = 0xe,

        .extended_family = 0x0,
        .family          = 0x6
        // 	Family 6 Model 78
    },

    {   // Broadwell (Client)
        .arch = CLIENT_arch(Broadwell),
        .core = 0,  // C, W, H

        .extended_model  = 0x4,
        .model           = 0x7,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 71
    }, { // Broadwell (Client)
        .arch = CLIENT_arch(Broadwell),
        .core = 0,  // U, Y, S

        .extended_model  = 0x3,
        .model           = 0xD,

        .extended_family = 0x0,
        .family          = 0x6
        //	Family 6 Model 61
    },

    { // Haswell (Client)
        .arch = CLIENT_arch(Haswell),
        .core = 0,  // GT3E

        .extended_model  = 0x4,
        .model           = 0x6,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 70
    }, { // Haswell (Client)
        .arch = CLIENT_arch(Haswell),
        .core = 0,  // ULT

        .extended_model  = 0x4,
        .model           = 0x5,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 69
    }, { // Haswell (Client)
        .arch = CLIENT_arch(Haswell),
        .core = 0,  // S

        .extended_model  = 0x3,
        .model           = 0xc,

        .extended_family = 0x0,
        .family          = 0x6
        // Family 6 Model 60
    },

};

static char* micro_arch_strings[] = {
    string_arch(Netburst),
    string_arch(Knights_Ferry),
    string_arch(Knights_Corner),
    string_arch(Meteor_Lake),
    string_arch(Raptor_Lake),
    string_arch(Alder_Lake),
    string_arch(Rocket_Lake),
    string_arch(Tiger_Lake),
    string_arch(Ice_Lake_c),
    string_arch(Comet_Lake),
    string_arch(Amber_Lake),
    string_arch(Whiskey_Lake),
    string_arch(Cannon_Lake),
    string_arch(Coffee_Lake),
    string_arch(Kaby_Lake),
    string_arch(Skylake_c),
    string_arch(Broadwell_c),
    string_arch(Haswell_c),
    string_arch(Sandy_Bridge_c),
    string_arch(Westmere_c),
    string_arch(Nehalem_c),
    string_arch(Penryn_c),
    string_arch(Core_c),
    string_arch(Modified_Pentium_M),
    string_arch(Pentium_M),
    string_arch(P6_c),
    string_arch(Diamond_Rapids),
    string_arch(Granite_Rapids),
    string_arch(Emerald_Rapids),
    string_arch(Sapphire_Rapids),
    string_arch(Ice_Lake_c),
    string_arch(Cooper_Lake),
    string_arch(Cascade_Lake),
    string_arch(Skylake_s),
    string_arch(Broadwell_s),
    string_arch(Haswell_s),
    string_arch(Sandy_Bridge_s),
    string_arch(Westmere_s),
    string_arch(Nehalem_s),
    string_arch(Penryn_s),
    string_arch(P6_s),
    string_arch(Tremont),
    string_arch(Goldmont_Plus),
    string_arch(Goldmont),
    string_arch(Airmont),
    string_arch(Silvermont),
    string_arch(Saltwell),
    string_arch(Bonnell),
    string_arch(Knights_Mill),
    string_arch(Knights_Landing),
    string_arch(Lakemont),
    string_arch(P5),
    string_arch(i80486),
    string_arch(i80386),

    "Not Defined"
};

char* get_micro_arch_strings(micro_arch this_arch) {
    if (this_arch < i80386) return micro_arch_strings[this_arch];
    else return micro_arch_strings[i80386+1];
}

#include <stdio.h>

int main() {

    for (uint16_t i = 0; i < sizeof(cpu_models)/sizeof(cpu_models[0]); i++) {
        cpu_model *this_cpu = cpu_models + i;
        printf("Cpu model number: 0x%x\n", get_model_number(this_cpu->family, this_cpu->model, this_cpu->extended_model));
        printf(
            "\tID Core:            0x%x\n"
            "\tARCH:               %s\n"
            "\tID extended_family: 0x%x\n"
            "\tID family:          0x%x\n"
            "\tID extended_model:  0x%x\n"
            "\tID model:           0x%x\n",
            this_cpu->core, get_micro_arch_strings(this_cpu->arch),
            this_cpu->extended_family, this_cpu->family, 
            this_cpu->extended_model, this_cpu->model
        );
    }
    return 0;
}