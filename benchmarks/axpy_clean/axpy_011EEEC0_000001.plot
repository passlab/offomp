set title "Offloading (axpy_kernel) Profile on 6 Devices"
set yrange [0:72.000000]
set xlabel "execution time in ms"
set xrange [0:158.400000]
set style fill pattern 2 bo 1
set style rect fs solid 1 noborder
set border 15 lw 0.2
set xtics out nomirror
unset key
set ytics out nomirror ("dev 0(sysid:0,type:HOSTCPU)" 5,"dev 1(sysid:1,type:HOSTCPU)" 15,"dev 2(sysid:0,type:THSIM)" 25,"dev 3(sysid:1,type:THSIM)" 35,"dev 4(sysid:2,type:THSIM)" 45,"dev 5(sysid:3,type:THSIM)" 55)
set object 1 rect from 4, 65 to 17, 68 fc rgb "#FF0000"
set label "ACCU_TOTAL" at 4,63 font "Helvetica,8'"

set object 2 rect from 21, 65 to 34, 68 fc rgb "#00FF00"
set label "INIT_0" at 21,63 font "Helvetica,8'"

set object 3 rect from 38, 65 to 51, 68 fc rgb "#0000FF"
set label "INIT_0.1" at 38,63 font "Helvetica,8'"

set object 4 rect from 55, 65 to 68, 68 fc rgb "#FFFF00"
set label "INIT_1" at 55,63 font "Helvetica,8'"

set object 5 rect from 72, 65 to 85, 68 fc rgb "#00FFFF"
set label "MODELING" at 72,63 font "Helvetica,8'"

set object 6 rect from 89, 65 to 102, 68 fc rgb "#FF00FF"
set label "ACC_MAPTO" at 89,63 font "Helvetica,8'"

set object 7 rect from 106, 65 to 119, 68 fc rgb "#808080"
set label "KERN" at 106,63 font "Helvetica,8'"

set object 8 rect from 123, 65 to 136, 68 fc rgb "#800000"
set label "PRE_BAR_X" at 123,63 font "Helvetica,8'"

set object 9 rect from 140, 65 to 153, 68 fc rgb "#808000"
set label "DATA_X" at 140,63 font "Helvetica,8'"

set object 10 rect from 157, 65 to 170, 68 fc rgb "#008000"
set label "POST_BAR_X" at 157,63 font "Helvetica,8'"

set object 11 rect from 174, 65 to 187, 68 fc rgb "#800080"
set label "ACC_MAPFROM" at 174,63 font "Helvetica,8'"

set object 12 rect from 191, 65 to 204, 68 fc rgb "#008080"
set label "FINI_1" at 191,63 font "Helvetica,8'"

set object 13 rect from 208, 65 to 221, 68 fc rgb "#000080"
set label "BAR_FINI_2" at 208,63 font "Helvetica,8'"

set object 14 rect from 225, 65 to 238, 68 fc rgb "(null)"
set label "PROF_BAR" at 225,63 font "Helvetica,8'"

set object 15 rect from 0.281757, 0 to 156.996730, 10 fc rgb "#FF0000"
set object 16 rect from 0.116104, 0 to 61.499546, 10 fc rgb "#00FF00"
set object 17 rect from 0.118127, 0 to 61.911503, 10 fc rgb "#0000FF"
set object 18 rect from 0.118687, 0 to 62.211199, 10 fc rgb "#FFFF00"
set object 19 rect from 0.119268, 0 to 62.748466, 10 fc rgb "#FF00FF"
set object 20 rect from 0.120306, 0 to 71.472653, 10 fc rgb "#808080"
set object 21 rect from 0.137022, 0 to 71.759823, 10 fc rgb "#800080"
set object 22 rect from 0.137809, 0 to 72.028196, 10 fc rgb "#008080"
set object 23 rect from 0.138073, 0 to 146.761000, 10 fc rgb "#000080"
set arrow from  0,0 to 158.400000,0 nohead
set object 24 rect from 0.278060, 10 to 156.447451, 20 fc rgb "#FF0000"
set object 25 rect from 0.035216, 10 to 20.005221, 20 fc rgb "#00FF00"
set object 26 rect from 0.038749, 10 to 20.653177, 20 fc rgb "#0000FF"
set object 27 rect from 0.039677, 10 to 21.670805, 20 fc rgb "#FFFF00"
set object 28 rect from 0.041681, 10 to 22.537527, 20 fc rgb "#FF00FF"
set object 29 rect from 0.043301, 10 to 31.412613, 20 fc rgb "#808080"
set object 30 rect from 0.060292, 10 to 31.726407, 20 fc rgb "#800080"
set object 31 rect from 0.061285, 10 to 32.151420, 20 fc rgb "#008080"
set object 32 rect from 0.061783, 10 to 144.593137, 20 fc rgb "#000080"
set arrow from  0,10 to 158.400000,10 nohead
set object 33 rect from 0.280047, 20 to 163.580208, 30 fc rgb "#FF0000"
set object 34 rect from 0.071526, 20 to 38.233653, 30 fc rgb "#00FF00"
set object 35 rect from 0.073601, 20 to 38.703045, 30 fc rgb "#0000FF"
set object 36 rect from 0.074236, 20 to 40.376977, 30 fc rgb "#FFFF00"
set object 37 rect from 0.077473, 20 to 46.812171, 30 fc rgb "#FF00FF"
set object 38 rect from 0.089778, 20 to 55.429839, 30 fc rgb "#808080"
set object 39 rect from 0.106284, 20 to 55.990080, 30 fc rgb "#800080"
set object 40 rect from 0.107624, 20 to 56.528918, 30 fc rgb "#008080"
set object 41 rect from 0.108404, 20 to 145.812297, 30 fc rgb "#000080"
set arrow from  0,20 to 158.400000,20 nohead
set object 42 rect from 0.283538, 30 to 167.841282, 40 fc rgb "#FF0000"
set object 43 rect from 0.146273, 30 to 77.132488, 40 fc rgb "#00FF00"
set object 44 rect from 0.148090, 30 to 77.476049, 40 fc rgb "#0000FF"
set object 45 rect from 0.148495, 30 to 79.041906, 40 fc rgb "#FFFF00"
set object 46 rect from 0.151533, 30 to 88.150377, 40 fc rgb "#FF00FF"
set object 47 rect from 0.168963, 30 to 96.762824, 40 fc rgb "#808080"
set object 48 rect from 0.185445, 30 to 97.330901, 40 fc rgb "#800080"
set object 49 rect from 0.186812, 30 to 97.675504, 40 fc rgb "#008080"
set object 50 rect from 0.187202, 30 to 147.726407, 40 fc rgb "#000080"
set arrow from  0,30 to 158.400000,30 nohead
set object 51 rect from 0.286186, 40 to 165.726138, 50 fc rgb "#FF0000"
set object 52 rect from 0.236991, 40 to 124.381677, 50 fc rgb "#00FF00"
set object 53 rect from 0.238567, 40 to 124.687644, 50 fc rgb "#0000FF"
set object 54 rect from 0.238915, 40 to 126.109387, 50 fc rgb "#FFFF00"
set object 55 rect from 0.241644, 40 to 131.944659, 50 fc rgb "#FF00FF"
set object 56 rect from 0.252826, 40 to 140.522647, 50 fc rgb "#808080"
set object 57 rect from 0.269248, 40 to 141.042688, 50 fc rgb "#800080"
set object 58 rect from 0.270483, 40 to 141.340299, 50 fc rgb "#008080"
set object 59 rect from 0.270822, 40 to 149.204549, 50 fc rgb "#000080"
set arrow from  0,40 to 158.400000,40 nohead
set object 60 rect from 0.284847, 50 to 165.676539, 60 fc rgb "#FF0000"
set object 61 rect from 0.194010, 50 to 101.947005, 60 fc rgb "#00FF00"
set object 62 rect from 0.195603, 50 to 102.262891, 60 fc rgb "#0000FF"
set object 63 rect from 0.195975, 50 to 103.797417, 60 fc rgb "#FFFF00"
set object 64 rect from 0.198915, 50 to 110.245137, 60 fc rgb "#FF00FF"
set object 65 rect from 0.211268, 50 to 118.756301, 60 fc rgb "#808080"
set object 66 rect from 0.227558, 50 to 119.265893, 60 fc rgb "#800080"
set object 67 rect from 0.228782, 50 to 119.592221, 60 fc rgb "#008080"
set object 68 rect from 0.229169, 50 to 148.503329, 60 fc rgb "#000080"
set arrow from  0,50 to 158.400000,50 nohead
plot 0
