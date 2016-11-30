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

set object 15 rect from 0.120520, 0 to 160.757271, 10 fc rgb "#FF0000"
set object 16 rect from 0.100253, 0 to 133.367611, 10 fc rgb "#00FF00"
set object 17 rect from 0.102519, 0 to 134.103849, 10 fc rgb "#0000FF"
set object 18 rect from 0.102862, 0 to 135.137718, 10 fc rgb "#FFFF00"
set object 19 rect from 0.103646, 0 to 136.471824, 10 fc rgb "#FF00FF"
set object 20 rect from 0.104674, 0 to 137.057944, 10 fc rgb "#808080"
set object 21 rect from 0.105109, 0 to 137.757632, 10 fc rgb "#800080"
set object 22 rect from 0.105895, 0 to 138.416854, 10 fc rgb "#008080"
set object 23 rect from 0.106152, 0 to 156.638770, 10 fc rgb "#000080"
set arrow from  0,0 to 158.400000,0 nohead
set object 24 rect from 0.118981, 10 to 159.113788, 20 fc rgb "#FF0000"
set object 25 rect from 0.086024, 10 to 114.833707, 20 fc rgb "#00FF00"
set object 26 rect from 0.088319, 10 to 115.705707, 20 fc rgb "#0000FF"
set object 27 rect from 0.088753, 10 to 116.666473, 20 fc rgb "#FFFF00"
set object 28 rect from 0.089518, 10 to 118.253825, 20 fc rgb "#FF00FF"
set object 29 rect from 0.090731, 10 to 118.850388, 20 fc rgb "#808080"
set object 30 rect from 0.091194, 10 to 119.646674, 20 fc rgb "#800080"
set object 31 rect from 0.092038, 10 to 120.412937, 20 fc rgb "#008080"
set object 32 rect from 0.092361, 10 to 154.499237, 20 fc rgb "#000080"
set arrow from  0,10 to 158.400000,10 nohead
set object 33 rect from 0.117359, 20 to 156.747111, 30 fc rgb "#FF0000"
set object 34 rect from 0.073147, 20 to 97.831023, 30 fc rgb "#00FF00"
set object 35 rect from 0.075357, 20 to 98.661251, 30 fc rgb "#0000FF"
set object 36 rect from 0.075693, 20 to 99.751249, 30 fc rgb "#FFFF00"
set object 37 rect from 0.076531, 20 to 101.148015, 30 fc rgb "#FF00FF"
set object 38 rect from 0.077599, 20 to 101.671475, 30 fc rgb "#808080"
set object 39 rect from 0.078011, 20 to 102.380301, 30 fc rgb "#800080"
set object 40 rect from 0.078819, 20 to 103.143955, 30 fc rgb "#008080"
set object 41 rect from 0.079147, 20 to 152.562041, 30 fc rgb "#000080"
set arrow from  0,20 to 158.400000,20 nohead
set object 42 rect from 0.115842, 30 to 154.920874, 40 fc rgb "#FF0000"
set object 43 rect from 0.059039, 30 to 79.408078, 40 fc rgb "#00FF00"
set object 44 rect from 0.061206, 30 to 80.199144, 40 fc rgb "#0000FF"
set object 45 rect from 0.061553, 30 to 81.376603, 40 fc rgb "#FFFF00"
set object 46 rect from 0.062464, 30 to 82.845165, 40 fc rgb "#FF00FF"
set object 47 rect from 0.063599, 30 to 83.409092, 40 fc rgb "#808080"
set object 48 rect from 0.064026, 30 to 84.116613, 40 fc rgb "#800080"
set object 49 rect from 0.064832, 30 to 84.886792, 40 fc rgb "#008080"
set object 50 rect from 0.065144, 30 to 150.466893, 40 fc rgb "#000080"
set arrow from  0,30 to 158.400000,30 nohead
set object 51 rect from 0.114245, 40 to 152.930164, 50 fc rgb "#FF0000"
set object 52 rect from 0.045021, 40 to 61.460294, 50 fc rgb "#00FF00"
set object 53 rect from 0.047466, 40 to 62.300964, 50 fc rgb "#0000FF"
set object 54 rect from 0.047839, 40 to 63.424904, 50 fc rgb "#FFFF00"
set object 55 rect from 0.048706, 40 to 64.889549, 50 fc rgb "#FF00FF"
set object 56 rect from 0.049852, 40 to 65.506998, 50 fc rgb "#808080"
set object 57 rect from 0.050297, 40 to 66.239320, 50 fc rgb "#800080"
set object 58 rect from 0.051113, 40 to 66.989918, 50 fc rgb "#008080"
set object 59 rect from 0.051473, 40 to 148.320834, 50 fc rgb "#000080"
set arrow from  0,40 to 158.400000,40 nohead
set object 60 rect from 0.112425, 50 to 155.727611, 60 fc rgb "#FF0000"
set object 61 rect from 0.023494, 50 to 34.436143, 60 fc rgb "#00FF00"
set object 62 rect from 0.026869, 50 to 36.031330, 60 fc rgb "#0000FF"
set object 63 rect from 0.027744, 50 to 39.754295, 60 fc rgb "#FFFF00"
set object 64 rect from 0.030618, 50 to 42.519109, 60 fc rgb "#FF00FF"
set object 65 rect from 0.032700, 50 to 43.521647, 60 fc rgb "#808080"
set object 66 rect from 0.033563, 50 to 44.688663, 60 fc rgb "#800080"
set object 67 rect from 0.034851, 50 to 46.125896, 60 fc rgb "#008080"
set object 68 rect from 0.035492, 50 to 145.441148, 60 fc rgb "#000080"
set arrow from  0,50 to 158.400000,50 nohead
plot 0
