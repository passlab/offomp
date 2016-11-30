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

set object 15 rect from 0.246181, 0 to 154.248653, 10 fc rgb "#FF0000"
set object 16 rect from 0.096566, 0 to 59.314690, 10 fc rgb "#00FF00"
set object 17 rect from 0.098892, 0 to 59.820922, 10 fc rgb "#0000FF"
set object 18 rect from 0.099500, 0 to 60.296456, 10 fc rgb "#FFFF00"
set object 19 rect from 0.100290, 0 to 60.890572, 10 fc rgb "#FF00FF"
set object 20 rect from 0.101271, 0 to 65.473151, 10 fc rgb "#808080"
set object 21 rect from 0.108909, 0 to 65.818665, 10 fc rgb "#800080"
set object 22 rect from 0.109720, 0 to 66.134684, 10 fc rgb "#008080"
set object 23 rect from 0.110015, 0 to 147.770560, 10 fc rgb "#000080"
set arrow from  0,0 to 158.400000,0 nohead
set object 24 rect from 0.244104, 10 to 153.357779, 20 fc rgb "#FF0000"
set object 25 rect from 0.072561, 10 to 45.068546, 20 fc rgb "#00FF00"
set object 26 rect from 0.075256, 10 to 45.603070, 20 fc rgb "#0000FF"
set object 27 rect from 0.075879, 10 to 46.138798, 20 fc rgb "#FFFF00"
set object 28 rect from 0.076810, 10 to 46.883399, 20 fc rgb "#FF00FF"
set object 29 rect from 0.078028, 10 to 51.638133, 20 fc rgb "#808080"
set object 30 rect from 0.085944, 10 to 52.000502, 20 fc rgb "#800080"
set object 31 rect from 0.086832, 10 to 52.397181, 20 fc rgb "#008080"
set object 32 rect from 0.087185, 10 to 146.407765, 20 fc rgb "#000080"
set arrow from  0,10 to 158.400000,10 nohead
set object 33 rect from 0.251441, 20 to 160.217502, 30 fc rgb "#FF0000"
set object 34 rect from 0.214988, 20 to 130.402147, 30 fc rgb "#00FF00"
set object 35 rect from 0.216995, 20 to 130.788593, 30 fc rgb "#0000FF"
set object 36 rect from 0.217392, 20 to 132.671466, 30 fc rgb "#FFFF00"
set object 37 rect from 0.220526, 20 to 134.743348, 30 fc rgb "#FF00FF"
set object 38 rect from 0.223974, 20 to 139.316295, 30 fc rgb "#808080"
set object 39 rect from 0.231577, 20 to 139.711169, 30 fc rgb "#800080"
set object 40 rect from 0.232481, 20 to 140.094605, 30 fc rgb "#008080"
set object 41 rect from 0.232875, 20 to 151.030072, 30 fc rgb "#000080"
set arrow from  0,20 to 158.400000,20 nohead
set object 42 rect from 0.241723, 30 to 156.366282, 40 fc rgb "#FF0000"
set object 43 rect from 0.037355, 30 to 24.678388, 40 fc rgb "#00FF00"
set object 44 rect from 0.041447, 30 to 25.540969, 40 fc rgb "#0000FF"
set object 45 rect from 0.042560, 30 to 28.220812, 40 fc rgb "#FFFF00"
set object 46 rect from 0.047055, 30 to 30.801334, 40 fc rgb "#FF00FF"
set object 47 rect from 0.051296, 30 to 35.588573, 40 fc rgb "#808080"
set object 48 rect from 0.059268, 30 to 36.088786, 40 fc rgb "#800080"
set object 49 rect from 0.060483, 30 to 36.899601, 40 fc rgb "#008080"
set object 50 rect from 0.061443, 30 to 144.799378, 40 fc rgb "#000080"
set arrow from  0,30 to 158.400000,30 nohead
set object 51 rect from 0.247946, 40 to 158.138999, 50 fc rgb "#FF0000"
set object 52 rect from 0.186323, 40 to 113.300394, 50 fc rgb "#00FF00"
set object 53 rect from 0.188623, 40 to 113.765694, 50 fc rgb "#0000FF"
set object 54 rect from 0.189112, 40 to 115.661810, 50 fc rgb "#FFFF00"
set object 55 rect from 0.192270, 40 to 117.651226, 50 fc rgb "#FF00FF"
set object 56 rect from 0.195571, 40 to 122.262095, 50 fc rgb "#808080"
set object 57 rect from 0.203240, 40 to 122.652154, 50 fc rgb "#800080"
set object 58 rect from 0.204138, 40 to 123.032580, 50 fc rgb "#008080"
set object 59 rect from 0.204511, 40 to 148.855861, 50 fc rgb "#000080"
set arrow from  0,40 to 158.400000,40 nohead
set object 60 rect from 0.249637, 50 to 180.996515, 60 fc rgb "#FF0000"
set object 61 rect from 0.119569, 50 to 73.184621, 60 fc rgb "#00FF00"
set object 62 rect from 0.121957, 50 to 73.677610, 60 fc rgb "#0000FF"
set object 63 rect from 0.122516, 50 to 75.681473, 60 fc rgb "#FFFF00"
set object 64 rect from 0.125857, 50 to 77.982092, 60 fc rgb "#FF00FF"
set object 65 rect from 0.129677, 50 to 103.750597, 60 fc rgb "#808080"
set object 66 rect from 0.172518, 50 to 104.387449, 60 fc rgb "#800080"
set object 67 rect from 0.173829, 50 to 104.975546, 60 fc rgb "#008080"
set object 68 rect from 0.174523, 50 to 149.950791, 60 fc rgb "#000080"
set arrow from  0,50 to 158.400000,50 nohead
plot 0
