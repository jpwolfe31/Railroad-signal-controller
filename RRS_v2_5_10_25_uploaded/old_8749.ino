/*   For historical reference - used 1995 to 2025
     Intel 8749H assembly language wrk output file

2			;  RRS12.ASM
3			;  John Wolfe
4			;  March 10, 2019
5			;
6			;
7			; --------------------------------------
8			;  Railroad Signal Controller
9			; --------------------------------------
10			;
11			; ---------------------------------
12			;  input/output structure
13			; ---------------------------------
14			;
15			;  Power relay       = P2.5 (pin 36) active low (Note - now disabled
16			;                             as using AC toggle switch)
17			;  Red led           = P2.6 (pin 37)
18			;  Sample rate
19			;    output test     = P2.7 (pin 38)
20			;
21			;  SW input          = t0 (pin 1) active low - 10K pull-up to 5V
22			;  IR input          = t1 (pin 39) controlled by Sony remote set to VCR 
23			;                                     sensor decodes Sony code on 38 KHz 
24			;                                     carrier
25			;
26			;  P2.0 through P2.4 handle two 8243 port expanders 0Px.x and 1Px.x
27			;    data            = P2.0  (pin 21)
28			;    data            = P2.1  (pin 22)
29			;    data            = P2.2  (pin 23)
30			;    data            = P2.3  (pin 24)
31			;    cs0!/cs1        = P2.4  (pin 35) (inversion through transistor)
32			;
33			;  sls0
34			;    Red led         = BUS.0 (pin 12) active low 
35			;    Green led       = BUS.1 (pin 13) active low
36			;    Yellow          = BUS.2 (pin 14) 
37			;    Green           = BUS.3 (pin 15) 
38			;
39			;  sls1
40			;    Red led         = BUS.4 (pin 16) active low
41			;    Green led       = BUS.5 (pin 17) active low
42			;    Yellow          = BUS.6 (pin 18) 
43			;    Green           = BUS.7 (pin 19) 
44			;
45			;  sls2
46			;    Red led         = P1.0  (pin 27) active low
47			;    Green led       = P1.1  (pin 28) active low
48			;    Yellow          = P1.2  (pin 29) 
49			;    Green           = P1.3  (pin 30)
50			;
51			;  sls3
52			;    Red led         = P1.4  (pin 31) active low
53			;    Green led       = P1.5  (pin 32) active low
54			;    Yellow          = P1.6  (pin 33) 
55			;    Green           = P1.7  (pin 34)
56			;
57			;  gcs4
58			;    Left led        = 0P4.0 (pin 2) active low
59			;    Right led       = 0P4.1 (pin 3) active low
60			;    Bell            = 0P4.2 (pin 4) active low 
61			;
62			;  gy5
63			;    Green led       = 0P5.0 (pin 1) 
64			;    Red led         = 0P5.1 (pin 23) 
65			;
66			;  gy6
67			;    Green led       = 0P6.0 (pin 20) 
68			;    Yellow led      = 0P6.1 (pin 19)  
69			;
70			;  cls7
71			;    Red led         = 0P7.0 (pin 13) active low
72			;    Yellow led      = 0P7.1 (pin 14) active low
73			;    Green led       = 0P7.2 (pin 15) active low 
74			;
75			;  cls8
76			;    Red led         = 1P4.0 (pin 2) active low
77			;    Yellow led      = 1P4.1 (pin 3) active low
78			;    Green led       = 1P4.2 (pin 4) active low 
79			;
80			;  cls9
81			;    Red led         = 1P5.0 (pin 1) active low
82			;    Yellow led      = 1P5.1 (pin 23) active low
83			;    Green led       = 1P5.2 (pin 22) active low 
84			;
85			;  cls10
86			;    Red led         = 1P6.0 (pin 20) active low
87			;    Green led       = 1P6.1 (pin 19) active low 
88			;
89			;  ws11
90			;    Green led       = 1P7.0 (pin 13) 
91			;    Red led         = 1P7.1 (pin 14) 
92			;
93			;  ws12
94			;    Green led       = 1P7.2 (pin 15) 
95			;    Red led         = 1P7.3 (pin 16)  
96			;
97			;
98			; ---------------------------------
99			;  construction
100			; ---------------------------------
101			;
102			;  Logic Board Terminals
103			;
104			;   LBT
105			;   _______________________
106			;  |                       |
107			;  | 2    4    6    8   10 |
108			;  |                       |
109			;  | 1    3    5    7    9 |
110			; _|_______________________|_
111			;
112			;   1 G  [used when outside of signal box
113			;   2 5V   otherwise powered from 5V supply on Logic Board 0]
114			;   3 SW (momentary) ground 
115			;   4 SW (momentary) pullled up through 10K resistor to 5V
116			;   5 IR 5V    
117			;   6 IR G
118			;   7 IR signal
119			;   8 NC
120			;   9 NC
121			;  10 NC
122			;
123			;  IR sensor TSOP4338 38 KHz modulation
124			;     Facing sensor pins are left to right on bottom S G 5V
125			;
126			;  Reset tied to ground with 1uF capacitor for power on
127			;    reset (pin 4)
128			;
129			;  8749 pins
130			;
131			;    T0    1      40  VCC
132			;    XTAL1 2      39  T1
133			;    XTAL2 3      38  P27
134			;    RST!  4      37  P26
135			;    SS!   5      36  P25
136			;    INT!  6      35  P24
137			;    EA    7      34  P17
138			;    RD!   8      33  P16
139			;    PSEN! 9      32  P15
140			;    WR!   10     31  P14
141			;    ALE   11     30  P13
142			;    D0    12     29  P12
143			;    D1    13     28  P11
144			;    D2    14     27  P10
145			;    D3    15     26  VDD
146			;    D4    16     25  PROG
147			;    D5    17     24  P23
148			;    D6    18     23  P22
149			;    D7    19     22  P21
150			;    G     20     21  P20
151			;
152			;  8243 pins
153			;
154			;    5.0   1      24  VCC
155			;    4.0   2      23  5.1
156			;    4.1   3      22  5.2
157			;    4.2   4      21  5.3
158			;    4.3   5      20  6.0
159			;    CS!   6      19  6.1
160			;    PROG  7      18  6.2
161			;    2.3   8      17  6.3
162			;    2.2   9      16  7.3
163			;    2.1   10     15  7.2
164			;    2.0   11     14  7.1
165			;    G     12     13  7.0
166			;
167			;  searchlight (sls0, sls1, sls2, sls3)
168			;
169			;  leds
170			;    bicolor leds controlled by two 3906 pnp each through 
171			;      10K resistor (active low)
172			;    red led connected to pnp and +5 volts with 510 resistor
173			;    green led connected to pnp and +5 volts with 150 resistor
174			;
175			;  grade crossing (gcs4) (active low outputs)
176			;
177			;  leds
178			;    red leds for lights connected directly to outputs and +5 volts
179			;      through 750 resistor
180			;    green led for bell connected directly to outputs and +5 volts
181			;      through 1200 resistor 
182			;
183			;  gyralights (gy5, gy6)
184			;
185			;  leds
186			;    leds controlled by 3904 npn driven by 10K resistor
187			;    green leds for motors connected to npn and +5 volts through 1K resistor
188			;    red led for red light connected to npn and +5 volts through 510 resistor
189			;    yellow led for white lights connected to npn and +5 volts through 1k
190			;      resistor
191			;
192			;  color light signals (cls7, cls8, cls9, cls10) (active low outputs)
193			;
194			;  leds
195			;    red leds for lights connected directly to outputs and +5 volts
196			;      through 750 resistor
197			;    green and yellow leds connected directly to outputs and +5 volts
198			;      through 1K resistor 
199			;
200			;  wig-wags (ws11, ws12)
201			;
202			;  leds
203			;    leds controlled by 3904 npn driven by 10K resistor
204			;    green leds for motors connected to npn and +5 volts through 1K resistor
205			;    red leds for red lights connected to npn and +5 volts through 510 
206			;      resistor
207			;
208			;
209			;  Logic Connectors
210			;
211			;   LC0
212			;   ________________________________________________________________
213			;  |                                                                |
214			;  | 1    2    3    4    5    6    7    8    9    10   11   12   13 |
215			;  |                                                                |
216			;  | 14   15   16   17   18   19   20   21   22   23   24   25   26 |
217			; _|________________________________________________________________|_
218			;
219			;    logic for signals 0 through 6
220			;
221			;    1 sls0rl - BUS.0 (pin 12)
222			;    2 sls0gl - BUS.1 (pin 13) 
223			;    3 sls0y  - BUS.2 (pin 14) 
224			;    4 sls0g  - BUS.3 (pin 15)          
225			;    5 sls1rl - BUS.4 (pin 16)
226			;    6 sls1gl - BUS.5 (pin 17)
227			;    7 sls1y  - BUS.6 (pin 18)
228			;    8 sls1g  - BUS.7 (pin 19)
229			;    9 sls2rl - P1.0  (pin 27)
230			;   10 sls2gl - P1.1  (pin 28)
231			;   11 sls2y  - P1.2  (pin 29)
232			;   12 sls2g  - P1.3  (pin 30)
233			;   13 sls3rl - P1.4  (pin 31)
234			;   14 sls3gl - P1.5  (pin 32)
235			;   15 sls3y  - P1.6  (pin 33)
236			;   16 sls3g  - P1.7  (pin 34)
237			;   17 gcs4l  - 0P4.0 (pin 2)
238			;   18 gcs4r  - 0P4.1 (pin 3)
239			;   19 gcs4b  - 0P4.2 (pin 4)
240			;   20 +5
241			;   21 gy5m   - 3904 npn collector driven by 0P5.0 (pin 1)
242			;   22 gy5l   - 3904 npn collector driven by 0P5.1 (pin 23)
243			;   23 gy6m   - 3904 npn collector driven by 0P6.0 (pin 20)
244			;   24 gy6l   - 3904 npn collector driven by 0P6.1 (pin 19)
245			;   25 G
246			;   26 Power relay output = P2.5 (pin 36) active low 
247			;       (Note - now disabled as using AC toggle switch)
248			;   (Note - extra tip120 on board below gcs4 TIP120s)
249			;   (Note - gy5 and gy6 driven by LED drivers on logic board)
250			;
251			;
252			;   LC1
253			;   ________________________________________________________________
254			;  |                                                                |
255			;  | 1    2    3    4    5    6    7    8    9    10   11   12   13 |
256			;  |                                                                |
257			;  | 14   15   16   17   18   19   20   21   22   23   24   25   26 |
258			; _|________________________________________________________________|_
259			;
260			;    logic for signals 7 through 12
261			;
262			;    1 cls7r  - 0P7.0 (pin 13) (active low)
263			;    2 cls7y  - 0P7.1 (pin 14) (active low)
264			;    3 cls7g  - 0P7.2 (pin 15) (active low)
265			;    4 cls8r  - 1P4.0 (pin 2) (active low)
266			;    5 cls8y  - 1P4.1 (pin 3) (active low)
267			;    6 cls8g  - 1P4.2 (pin 4) (active low)
268			;    7 cls9r  - 1P5.0 (pin 1) (active low)
269			;    8 cls9y  - 1P5.1 (pin 23) (active low)
270			;    9 cls9g  - 1P5.2 (pin 22) (active low)
271			;   10 cls10r - 1P6.0 (pin 20) (active low)
272			;   11 cls10g - 1P6.1 (pin 19) (active low)
273			;   12 ws11m - 3904 npn collector driven by 1P7.0 (pin 13
274			;   13 ws11l - 3904 npn collector driven by 1P7.1 (pin 14)
275			;   14 ws12m - 3904 npn collector driven by 1P7.2 (pin 15)
276			;   15 ws12l - 3904 npn collector driven by 1P7.3 (pin 16)
277			;   16 to 25 NC
278			;   26 G 
279			;
280			;
281			;  Searchlight drivers
282			;
283			;    nand gate generates signal for
284			;      sls lights from led control lines
285			;
286			;    74LS00 pins 
287			;
288			;      I0A   1  BUS.0     V   14  +5
289			;      I0B   2  BUS.1    I2A  13  P1.0
290			;      O0    3           I2B  12  P1.1
291			;      I1A   4  BUS.4    O2   11
292			;      I1B   5  BUS.5    I3A  10  P1.4
293			;      O1    6           I3B  9   P1.5
294			;      G     7  G        O3   8
295			;
296			;    nand gate ouputs O0, O1, O2 and O3 drives TIP 120 
297			;      npn though 1K ohm resistor
298			;
299			;    TIP120 pins facing device left to right on bottom B C E
300			;
301			;
302			;    L298(0) pins - drives sls0 and sls1 solenoids
303			;
304			;      CsenA 1   G       O1        2
305			;      O2    3           Vsource   4   +12
306			;      I1    5   BUS.2   EA        6   +5
307			;      I2    7   BUS.3   G         8   G
308			;      Vlog  9   +5      I3        10  BUS.6
309			;      EB    11  +5      I4        12  BUS.7
310			;      O3    13          O4        14
311			;      CsenB 15  G
312			;
313			;    L298(1) pins - drives sls2 and sls3 solenoids
314			;
315			;      CsenA 1   G       O1        2
316			;      O2    3           Vsource   4   +12
317			;      I1    5   P1.2    EA        6   +5
318			;      I2    7   P1.3    G         8   G
319			;      Vlog  9   +5      I3        10  P1.6
320			;      EB    11  +5      I4        12  P1.7
321			;      O3    13          O4        14
322			;      CsenB 15  G
323			;
324			;  Grade crossing drivers
325			;
326			;    signal drives 3904 npn through 10K resistor
327			;    collector of 3904 connected to +5 through 1K resistor
328			;      and to base of TIP 120 such that low level on npn
329			;      causes TIP 120 to turn on
330			;
331			;  Semaphore driver
332			;
333			;    input taken from searchlight signal output
334			;      yellow-green inputs on internal unit with diode 
335			;      so semaphore only operative when searchlight signal 
336			;      is green 
337			;
338			;  Gyralight driver 
339			;
340			;    driver signal taken from 3904 npn collector for led
341			;    driver takes diode of AC solid state relay to ground through
342			;      a resistor tied to +12
343			;
344			;  Colorlight driver
345			;
346			;    signal drives 3904 npn through 10K resistor
347			;    collector of 3904 connected to +5 through 1K resistor
348			;      and to base of TIP 120 such that low level on npn
349			;      causes TIP 120 to turn on
350			;    additional signals can be added in parallel by adding TIP120s
351			;
352			;  Wig-wag driver 
353			;
354			;    driver signal taken from 3904 npn collector for led
355			;    driver takes diode of AC solid state relay to ground through
356			;      a resistor tied to +12
357			;
358			;  Solid State Relays (mounted near top of cabinet left to right)
359			;    gy5 motor 60 volt DC green solid
360			;    wws11 AC green striped
361			;    wws12 AC blue solid
362			;    gy5 light AC blue striped
363			;    gy6 light AC brown solid
364			;    gy6 motor 60 volt DC brown striped
365			;
366			;  Power supplies
367			;    12 volt for middle board
368			;    12 volt for bottom board
369			;    60 volt for gy5 and gy6 motors
370			;    12+ volt IBM ps for wws11
371			;    12+ volt IBM ps for wws12
372			;    5 volt for top logic board 
373			;    (Note - 12 volt power supply for this board at its maximum.
374			;     Resistor in CLS9 adjusted to lower power requirements.)
375			;    (Note - 12 volt supplies turn on faster than 5 volt supply.
376			;     Therefor, all lights are iluminated at once on start up
377			;     since 8749 cannot turn off lights fast enough.  Puts stress
378			;     on start up of 12 volt supply powering the colot light signals.)
379			;
380			;
381			;  Signal Locations in Garage (left to right from signal box)
382			;
383			;    Unilens Dwarf (CLS10- cable)
384			;    USS H Dwarf with Phankill (SLS4- cable)
385			;    Coloright Three Aspect Triangle (CLS9 cable)
386			;    GRS SA Searchlight (first signal) (top) (SLS1 + cable) 
387			;    Colorlight Two Aspect homemade (bottom) (CLS10+ cable) 
388			;    Grade Crossing Signal and Bell (GCS5 Cable)
389			;    Wig Wag (11 cable)
390			;    USS H2 (top) (SLS1- cable)
391			;    USS H2 (bottom) (SLS2- cable)
392			;    Wig Wag (12 cable)
393			;    Gyralight Red (top) (6 cable, AC 6- cable)
394			;    Gyralight White (bottom) (6 cable, AC 6+ cable)
395			;    Wayside Signal (SLS3+ cable) (from SLS3- with diode) (green is down)
396			;    H2 Dwarf (SLS3-)
397			;    Colorlight Three Aspect Triangle (CLC7 cable)
398			;    Colorlight Three Aspect (CLS8 cable)
399			;    H2 Unit on workbench (SLS2+ cable)
400			;
401			;  Signal Connectors
402			;
403			;   SC0                                             top board view
404			;   ________________________________________________________________
405			;  |                                                                |
406			;  | 2    4    6    8    10   12   14   16   18   20   22   24   26 |
407			;  |                                                                |
408			;  | 1    3    5    7    9    11   13   15   17   19   21   23   25 |
409			; _|________________________________________________________________|_
410			;
411			;   SC0
412			;
413			;    connectors for signals 0 through 6
414			;
415			;    1 sls0v     - orange, brown and blue striped +12       Cable 1-, 1+
416			;    2 sls0s     - orange, brown and blue solid (TIP120 C)  Cable 1-, 1+
417			;    3 sls0y (r) - green striped (L298(0) pin 2)            Cable 1-, 1+
418			;    4 sls0g (l) - green solid (L298(0) pin 3)              Cable 1-, 1+
419			;    5 sls1v     - orange, brown and blue striped +12       Cable 2-, 2+
420			;    6 sls1s     - orange, brown and blue solid (TIP120 C)  Cable 2-, 2+
421			;    7 sls1y (r) - green striped (L298(0) pin 13)           Cable 2-, 2+
422			;    8 sls1g (l) - green solid (L298(0) pin 14              Cable 2-, 2+
423			;    9 sls2v     - orange, brown and blue striped +12       Cable 3-
424			;   10 sls2s     - orange, brown and blue solid (TIP120 C)  Cable 3-
425			;   11 sls2y (r) - green striped (L298(1) pin 2)            Cable 3-
426			;   12 sls2g (l) - green solid (L298(1) pin 3)              Cable 3-
427			;   13 sls3v     - orange, brown and blue striped +12       Cable 4
428			;   14 sls3s     - orange, brown and blue solid (TIP120 C)  Cable 4
429			;   15 sls3y (r) - green striped (L298(1) pin 13)           Cable 4
430			;   16 sls3g (l) - green solid (L298(1) pin 14)             Cable 4
431			;   17 gcs4ls    - green solid (TIP120 C)                   Cable 5
432			;   18 gcs4v     - green and orange striped +12             Cable 5
433			;   19 gcs4rs    - orange solid (TIP120 C)                  Cable 5
434			;   20 gcs4bv    - brown and blue striped +12               Cable 5
435			;   21 gcs4bs    - brown and blue solid (TIP120 C)          Cable 5
436			;   22 NC 
437			;   23 gy5m      - green solid (3904 C)                     Relay
438			;   24 gy5l      - blue striped (3904 C)                    Relay
439			;   25 gy6m      - brown striped (3904 C)                   Relay
440			;   26 gy6l      - brown solid (3904 C)                     Relay
441			;
442			;    From Relays
443			;      AC Cable 6- for gy5 light transformer connected to AC relay
444			;      AC cable 6+ for gy6 light transformer connected to AC relay
445			;      Cable 6 for gy5 motor orange solid (+60) and orange striped (G)
446			;      Cable 6 for gy6 motor blue solid (+60) and blue striped (G)
447			;
448			;
449			;   SC1                                             top board view
450			;   ________________________________________________________________
451			;  |                                                                |
452			;  | 2    4    6    8    10   12   14   16   18   20   22   24   26 |
453			;  |                                                                |
454			;  | 1    3    5    7    9    11   13   15   17   19   21   23   25 |
455			; _|________________________________________________________________|_
456			;
457			;   SC1
458			;
459			;    connectors for signals 7 through 10
460			;
461			;    1 cls7v     - brown solid and striped +12              Cable 7
462			;    2 cls7rs    - orange solid and striped (TIP120 C)      Cable 7   
463			;    3 cls7ys    - blue solid and striped (TIP120 C)        Cable 7   
464			;    4 cls7gs    - green solid and striped (TIP120 C)       Cable 7   
465			;    5 cls8v     - brown solid and striped +12              Cable 8
466			;    6 cls8rs    - orange solid and striped (TIP120 C)      Cable 8   
467			;    7 cls8ys    - blue solid and striped (TIP120 C)        Cable 8   
468			;    8 cls8gs    - green solid and striped (TIP120 C)       Cable 8   
469			;    9 cls9v     - brown solid and striped +12              Cable 9
470			;   10 cls9rs    - orange solid and striped (TIP120 C)      Cable 9   
471			;   11 cls9ys    - blue solid and striped (TIP120 C)        Cable 9   
472			;   12 cls9gs    - green solid and striped (TIP120 C)       Cable 9   
473			;   13 cls10v    - brown/blue solid and striped +!2         Cable 10, 10+
474			;   14 cls10rs   - orange solid and striped (TIP120 C)      Cable 10, 10+   
475			;   15 cls10gs   - green solid and striped (TIP120 C)       Cable 10, 10+   
476			;   16 NC
477			;   17 wws11m    - green white (3904 C)                     Relay
478			;   18 wws11v    - blue solid +12                           Cable 11
479			;   19 wws11l    - blue striped (3904 C)                    Cable 11 
480			;   20 NC
481			;   21 wws12m    - blue solid (3904 C)                      Relay
482			;   22 wws12v    - blue solid +12                           Cable 12
483			;   23 wws12l    - blue striped (3904 C)                    Cable 12 
484			;   24 NC
485			;   25 NC
486			;   26 NC
487			;
488			;    From Relays
489			;      Cable 11 for wws11
490			;         motor +12 brown solid, brown striped and orange solid
491			;         motor G green solid, green striped and orange striped
492			;          - only green stripped attached to reduce power - others
493			;             connected to each other
494			;      Cable 12 for wws12
495			;         motor +12 brown solid, brown striped and orange solid
496			;         motor G green solid, green striped and orange striped
497			;
498			;
499			;  AC switch circuits
500			;
501			;    Power relay (not operational - solid state relay out of circuit)
502			;
503			;         PC0-13          PC0-26
504			;                                    12V   
505			;         12V -----^^^^---
506			;                ss relay |           |
507			;                         15K         3K        
508			;                         |           |
509			;                          \          |
510			;                           |---------        
511			;                          /          |
512			;                         |            \
513			;                         |             |--1K--- Power relay control PC0-26
514			;                         |            /           (active low)
515			;                         |           |
516			;
517			;                         G           G
518			;
519			;
520			;
521			;  Power Connectors
522			;
523			;   PC0
524			;   ________________________________________________________________
525			;  |                                                                |
526			;  | 1    2    3    4    5    6    7    8    9    10   11   12   13 |
527			;  |                                                                |
528			;  | 14   15   16   17   18   19   20   21   22   23   24   25   26 |
529			; _|________________________________________________________________|_
530			;
531			;   PC0
532			;
533			;    1 12  
534			;    2 12
535			;    3 12
536			;    4 12
537			;    5 12
538			;    6 12
539			;    7 12
540			;    8 12
541			;    9 12
542			;   10 12
543			;   11 12
544			;   12 12
545			;   13 G
546			;   14 G
547			;   15 G
548			;   16 G
549			;   17 G
550			;   18 G
551			;   19 G
552			;   20 G
553			;   21 G
554			;   22 G
555			;   23 G
556			;   24 G
557			;   25 G
558			;   26 Power Relay Control (active low) (not in operation)
559			;
560			;   PC1
561			;   ________________________________________________________________
562			;  |                                                                |
563			;  | 1    2    3    4    5    6    7    8    9    10   11   12   13 |
564			;  |                                                                |
565			;  | 14   15   16   17   18   19   20   21   22   23   24   25   26 |
566			; _|________________________________________________________________|_
567			;
568			;    1 12
569			;    2 12
570			;    3 12 
571			;    4 12
572			;    5 12
573			;    6 12
574			;    7 12
575			;    8 12
576			;    9 12 
577			;   10 12
578			;   11 12
579			;   12 12
580			;   13 12
581			;   14 G  
582			;   15 G
583			;   16 G
584			;   17 G
585			;   18 G
586			;   19 G
587			;   20 G
588			;   21 G
589			;   22 G
590			;   23 G
591			;   24 G
592			;   25 G
593			;   26 G
594			;
595			; ---------------------------------
596			;  variables
597			; ---------------------------------
598			;  
599			;  mode
600			;  m20 = mode
601			;  m21 = mode loop counter
602			;  m22 = delay loop counter 0 - 1 to 256 seconds 
603			;  m23 = delay loop counter 1 - 4 minutes to 17 hours
604			;
605			;  output
606			;  m26 = bus output
607			;  m27 = port 1 output
608			;
609			;  input
610			;  m28 = input state - 0 = nibble 1 
611			;                      1 = digit one no input 
612			;                      2 = nibble 2 timeout
613			;                      3 = digit two no input
614			;                      4 = switch
615			;  m29 = input data register - msn, lsn
616			;  m2a = input counter
617			;  m2b = red led state
618			;
619			;  remote
620			;  m2e = remote msb data
621			;  m2f = remote lsb data
622			;
623			;  m60 to m6b = standard data set 0-b
624			;  m70 to m7b = alternate data set 0-b
625			;
626			;  Data format
627			;
628			;    bit  function
629			;    7    flash
630			;    6    phase 2    
631			;    5    phase 1
632			;    4    phase 0
633			;    3    output 3
634			;    2    output 2
635			;    1    output 1
636			;    0    output 0
637			;
638			;  Use spaces, not tabs.  Can replace tabs ^t with 5 spaces
639			;
640			;
641			; ---------------------------------
642			;  constants
643			; ---------------------------------
644			;                                                   
645			                              ; mode tests
646	52		const     cmm       52        ; maximum mode is 82 (0x52) which is off 
647	AD		const     cmmc      ad        ; input generates carry on test of 83 (0x53)
648			                              ;                       
649			                              ; sls0 - phase 1, 4
650	12		const     csls0r    12        ; red light on
651	13		const     csls0rd   13        ; red light off
652	14		const     csls0y    14        ; yellow light on    
653	19		const     csls0g    19        ; green light on    
654	C3		const     csls0rf   c3        ; red light off
655	C7		const     csls0yf   c7        ; yellow light off
656	CB		const     csls0gf   cb        ; green light off
657			                              ;
658			                              ; sls1 - phase 2, 5
659	22		const     csls1r    22        ; red light on
660	23		const     csls1rd   23        ; red light off
661	24		const     csls1y    24        ; yellow light on    
662	29		const     csls1g    29        ; green light on    
663	D3		const     csls1rf   d3        ; red light off
664	D7		const     csls1yf   d7        ; yellow light off
665	DB		const     csls1gf   db        ; green light off
666			                              ;
667			                              ; sls2 - phase 4, 1
668	42		const     csls2r    42        ; red light on
669	43		const     csls2rd   43        ; red light off
670	44		const     csls2y    44        ; yellow light on    
671	49		const     csls2g    49        ; green light on    
672	93		const     csls2rf   93        ; red light off
673	97		const     csls2yf   97        ; yellow light off
674	9B		const     csls2gf   9b        ; green light off
675			                              ;
676			                              ; sls3 - phase 0, 3
677	02		const     csls3r    02        ; red light on
678	03		const     csls3rd   03        ; red light off
679	04		const     csls3y    04        ; yellow light on    
680	09		const     csls3g    09        ; green light on    
681	B3		const     csls3rf   b3        ; red light off
682	B7		const     csls3yf   b7        ; yellow light off
683	BB		const     csls3gf   bb        ; green light off
684			                              ;
685			                              ; gcs4 - phase 0, 3 (active low)
686	86		const     cgcs4l    86        ; left
687	35		const     cgcs4r    35        ; right
688	07		const     cgcs4d    07        ; dark
689	82		const     cgcs4lb   82        ; left with bell
690	31		const     cgcs4rb   31        ; right with bell
691	33		const     cgcs4db   33        ; dark with bell
692			                              ;
693			                              ; gy5 - phase 0
694	01		const     cgy5mo    01        ; motor
695	02		const     cgy5lo    02        ; red light
696	03		const     cgy5mlo   03        ; red light and motor
697	00		const     cgy5mlc   00        ; red light and motor clear
698			                              ;
699			                              ; gy6 - phase 0
700	01		const     cgy6mo    01        ; motor
701	02		const     cgy6lo    02        ; clear light
702	03		const     cgy6mlo   03        ; clear light and motor
703	00		const     cgy6mlc   00        ; clear light and motor clear
704			                              ;
705			                              ; cls7 - phase 0, 3
706	0E		const     ccls7r    0e        ; red light on
707	0F		const     ccls7rd   0f        ; red light off
708	0D		const     ccls7y    0d        ; yellow light on    
709	0B		const     ccls7g    0b        ; green light on    
710	BF		const     ccls7rf   bf        ; red light off
711	BF		const     ccls7yf   bf        ; yellow light off
712	BF		const     ccls7gf   bf        ; green light off
713			                              ;
714			                              ; cls8 - phase 1, 4
715	1E		const     ccls8r    1e        ; red light on
716	1F		const     ccls8rd   1f        ; red light off
717	1D		const     ccls8y    1d        ; yellow light on    
718	1B		const     ccls8g    1b        ; green light on    
719	CF		const     ccls8rf   cf        ; red light off
720	CF		const     ccls8yf   cf        ; yellow light off
721	CF		const     ccls8gf   cf        ; green light off
722			                              ;
723			                              ; cls9 - phase 2, 5
724	2E		const     ccls9r    2e        ; red light on
725	2F		const     ccls9rd   2f        ; red light off
726	2D		const     ccls9y    2d        ; yellow light on    
727	2B		const     ccls9g    2b        ; green light on    
728	DF		const     ccls9rf   df        ; red light off
729	DF		const     ccls9yf   df        ; yellow light off
730	DF		const     ccls9gf   df        ; green light off
731			                              ;
732			                              ; cls10 - phase 1, 4
733	1E		const     ccls10r   1e        ; red light on
734	1F		const     ccls10rd  1f        ; red light off
735	1D		const     ccls10g   1d        ; green light on    
736	CF		const     ccls10rf  cf        ; red light off
737	CF		const     ccls10gf  cf        ; green light off
738			                              ;
739			                              ; ws 11 - phase 0
740	01		const     cws11mo   01        ; motor
741	02		const     cws11lo   02        ; red light
742	03		const     cws11mlo  03        ; red light and motor
743	00		const     cws11mlc  00        ; red light and motor clear
744			                              ;
745			                              ; ws 12 - phase 0
746	04		const     cws12mo   04        ; motor
747	08		const     cws12lo   08        ; red light
748	0C		const     cws12mlo  0c        ; red light and motor
749	00		const     cws12mlc  00        ; red light and motor clear
750			                              ;
751			;
752			;
753			; ----------------------------
754			;  reset routine
755			; ----------------------------
756			;
757			          org  00             ; 
758			;                             ;
759	000	2320	reset     mov  a,#20          ; turn off red led and power relay
760	002	3A	          outl p2,a           ;
761			                              ;
762	003	2333	          mov  a,#33          ; turn off searchlight signals (active low and
763	005	02	          outl bus,a          ;   active high) 0 and 1  
764	006	39	          outl p1,a           ; turn off searchlight signals 2 and 3
765			                              ;
766	007	9AEF	          anl  p2,#ef         ; select 8243-0
767	009	23FF	          mov  a,#ff          ; turn off grade crossing signal 4
768	00B	3C	          movd p4,a           ;
769			                              ;
770	00C	2300	          mov  a,#00          ; turn off gyralights 5 and 6 (active high)
771	00E	3D	          movd p5,a           ;
772	00F	3E	          movd p6,a           ;
773			                              ;
774	010	23FF	          mov  a,#ff          ; turn off color light signal 7 (active low)
775	012	3F	          movd p7,a           ; 
776			                              ; 
777	013	8A10	          orl  p2,#10         ; select 8243-1
778	015	23FF	          mov  a,#ff          ; turn off color light signals 8, 9 and 10 
779			                              ;   (active low)
780	017	3C	          movd p4,a           ;
781	018	3D	          movd p5,a           ;
782	019	3E	          movd p6,a           ;
783	01A	2300	          mov  a,#00          ; turn off wig-wags 11 and 12 (active high)
784	01C	3F	          movd p7,a           ;
785			                              ; update memory locations later
786			                              ;
787	01D	15	          dis  i              ; disable interrupts
788			                              ;
789	01E	F4CC	          call d025s          ; 0.25 second delay (power relay now disabled 
790			                              ;   - if used in future may want longer delay
791			                              ;   and may consider turning on some signal
792			                              ;   lights here to speed drain of power packs) 
793	020	9ADF	          anl  p2,#df         ; turn on power relay
794			                              ;
795	022	B87F	          mov  r0,#7f         ; clear memory
796	024	27	          clr  a              ;    
797	025	A0	reset0    mov  @r0,a          ; clear data at address in r0
798	026	E825	          djnz r0,#reset0     ;  
799			                              ;
800	028	2333	          mov  a,#33          ; update bus and port memory locations
801	02A	B826	          mov  r0,#26         ; 
802	02C	A0	          mov  @r0,a          ;          
803	02D	B827	          mov  r0,#27         ; 
804	02F	A0	          mov  @r0,a          ;          
805			                              ;
806	030	B820	          mov  r0,#20         ; set initial mode
807	032	B000	          mov  @r0,#00        ;  
808			                              ;
809	034	0436	          jmp  start          ; jump to start
810			;          
811			;
812			; ----------------------------
813			;  start routine
814			; ----------------------------
815			;
816	036	B820	start     mov  r0,#20         ; determine mode
817	038	F0	          mov  a,@r0          ; get mode 
818			                              ;
819	039	D352	          xrl  a,#cmm         ; if maximum mode - shut down
820	03B	963F	          jnz  start0         ;    (Note - remote inputs over max mode  
821			                              ;     are not accepted by remote subroutine)
822	03D	0400	          jmp  reset          ; turn off power relay
823			                              ;
824	03F	F0	start0    mov  a,@r0          ; get mode
825	040	03F8	          add  a,#f8          ; test if 0-7 Note - if this changes, 
826	042	F658	          jc   start1         ;   adjust subtraction below    
827	044	F0	          mov  a,@r0          ; get mode
828	045	E7	          rl   a              ;
829	046	F4BB	          call jumpa          ; decode
830			                              ;
831	048	2404	          jmp  md0            ; pattern       
832	04A	24AD	          jmp  md1            ; pattern
833	04C	444E	          jmp  md2            ; red      
834	04E	4462	          jmp  md3            ; red flash
835	050	4476	          jmp  md4            ; yellow
836	052	448A	          jmp  md5            ; yellow flash 
837	054	449E	          jmp  md6            ; green      
838	056	44B2	          jmp  md7            ; dark
839			                              ;
840			                              ; load return address into program counter
841			                              ;   and increment stack pointer
842	058	C7	start1    mov  a,psw          ; increment stack pointer bits 2-0
843	059	17	          inc  a              ;
844	05A	D7	          mov  psw,a          ; 
845	05B	5307	          anl  a,#07          ; get new stack pointer value
846	05D	E7	          rl   a              ; double value
847	05E	0306	          add  a,#06          ; add base value (8 - 2)
848	060	A8	          mov  r0,a           ; create stack pointer pointer
849	061	B002	          mov  @r0,#start2    ; store new page return value in lsb
 --- Warning line 849 - 11 bit label start2 being used as 8 bit constant
850	063	18	          inc  r0             ;                              
851	064	B001	          mov  @r0,#01        ; **** Note this is 1 as start2 now on  
852			                              ;   second page
853	066	B820	          mov  r0,#20         ; get mode
854	068	F0	          mov  a,@r0          ;  
855	069	03F8	          add  a,#f8          ; subtract 8, as modes 0-7 covered above
856	06B	E7	          rl   a              ; times two - each jump is two bytes
857			                              ; decode - jumps to subroutines will return
858			                              ;   to address just loaded into stack pointer
859	06C	F4BB	          call jumpa          ;
860			                              ;
861	06E	648E	          jmp  gcs4f          ; 8
862	070	64A5	          jmp  gcs4db         ;       
863	072	649C	          jmp  gcs4fb         ; 
864	074	6497	          jmp  gcs4d          ; 
865			                              ;
866	076	64AF	          jmp  gy5lo          ; 12
867	078	64AA	          jmp  gy5mo          ; 
868	07A	64B4	          jmp  gy5mlo         ; 
869	07C	64B9	          jmp  gy5mlc         ; 
870			                              ;
871	07E	64C3	          jmp  gy6lo          ; 16
872	080	64BE	          jmp  gy6mo          ; 
873	082	64C8	          jmp  gy6mlo         ; 
874	084	64CD	          jmp  gy6mlc         ; 
875			                              ;
876	086	8463	          jmp  ws11lo         ; 20
877	088	845A	          jmp  ws11mo         ; 
878	08A	846C	          jmp  ws11mlo        ; 
879	08C	8475	          jmp  ws11mlc        ; 
880			                              ;
881	08E	8487	          jmp  ws12lo         ; 24
882	090	847E	          jmp  ws12mo         ; 
883	092	8490	          jmp  ws12mlo        ; 
884	094	8499	          jmp  ws12mlc        ; 
885			                              ;
886	096	44D2	          jmp  sls0r          ; 28
887	098	44D7	          jmp  sls0rf         ;       
888	09A	44E0	          jmp  sls0y          ;       
889	09C	44E5	          jmp  sls0yf         ;       
890	09E	44EE	          jmp  sls0g          ;       
891	0A0	44F3	          jmp  sls0gf         ;       
892	0A2	44FC	          jmp  sls0d          ;       
893			                              ;          
894	0A4	6401	          jmp  sls1r          ; 35
895	0A6	6406	          jmp  sls1rf         ;       
896	0A8	640F	          jmp  sls1y          ;       
897	0AA	6414	          jmp  sls1yf         ;       
898	0AC	641D	          jmp  sls1g          ;       
899	0AE	6422	          jmp  sls1gf         ;       
900	0B0	642B	          jmp  sls1d          ;
901			                              ;
902	0B2	6430	          jmp  sls2r          ; 42
903	0B4	6435	          jmp  sls2rf         ;
904	0B6	643E	          jmp  sls2y          ;       
905	0B8	6443	          jmp  sls2yf         ;       
906	0BA	644C	          jmp  sls2g          ;       
907	0BC	6451	          jmp  sls2gf         ;       
908	0BE	645A	          jmp  sls2d          ;
909			                              ;
910	0C0	645F	          jmp  sls3r          ; 49
911	0C2	6464	          jmp  sls3rf         ;
912	0C4	646D	          jmp  sls3y          ;       
913	0C6	6472	          jmp  sls3yf         ;       
914	0C8	647B	          jmp  sls3g          ;       
915	0CA	6480	          jmp  sls3gf         ;       
916	0CC	6489	          jmp  sls3d          ;
917			                              ;
918	0CE	E4D7	          jmp  cls7r          ; 56
919	0D0	64D2	          jmp  cls7rf         ;
920	0D2	E4DC	          jmp  cls7y          ;       
921	0D4	E4E1	          jmp  cls7yf         ;       
922	0D6	E4EA	          jmp  cls7g          ;       
923	0D8	E4EF	          jmp  cls7gf         ;       
924	0DA	64DB	          jmp  cls7d          ;
925			                              ;
926	0DC	64E0	          jmp  cls8r          ; 63
927	0DE	64E5	          jmp  cls8rf         ;
928	0E0	64EE	          jmp  cls8y          ;       
929	0E2	64F3	          jmp  cls8yf         ;       
930	0E4	64FC	          jmp  cls8g          ;       
931	0E6	8401	          jmp  cls8gf         ;       
932	0E8	840A	          jmp  cls8d          ;
933			                              ;
934	0EA	840F	          jmp  cls9r          ; 70
935	0EC	8414	          jmp  cls9rf         ;
936	0EE	E4F8	          jmp  cls9y          ;       
937	0F0	841D	          jmp  cls9yf         ;       
938	0F2	8426	          jmp  cls9g          ;       
939	0F4	842B	          jmp  cls9gf         ;       
940	0F6	8434	          jmp  cls9d          ;
941			                              ;
942	0F8	8439	          jmp  cls10r         ; 77
943	0FA	843E	          jmp  cls10rf        ; 78
944	0FC	8447	          jmp  cls10g         ; 79      
945	0FE	844C	          jmp  cls10gf        ; 80      
946	100	8455	          jmp  cls10d         ; 81
947			                              ;
948			                              ; 82 is off (since power relay now
949			                              ;    disconnected, now just reset)
950			                              ;
951	102	44CE	start2    jmp  mdx            ;
952			;
953			;
954			; ----------------------------
955			;  mode 0
956			; ----------------------------
957			;
958			;  m21 = mode loop counter
959			;
960	104	B821	md0       mov  r0,#21         ; set mode loop counter to 43
961	106	B02B	          mov  @r0,#2b        ;   (82*43 = 1hour)
962			                              ;
963	108	74AF	          call gy5lo          ; turn on lights on gyralights 5 and 6
964	10A	74C3	          call gy6lo          ;
965	10C	9463	          call ws11lo         ; turn on lights on wig-wags 11 and 12
966	10E	9487	          call ws12lo         ;                    
967			                              ;
968	110	54D2	md0a      call sls0r          ;
969	112	F4D7	          call cls7r          ;
970	114	7401	          call sls1r          ;
971	116	74E0	          call cls8r          ;
972	118	7430	          call sls2r          ;
973	11A	940F	          call cls9r          ;
974	11C	745F	          call sls3r          ;
975	11E	9439	          call cls10r         ;
976	120	748E	          call gcs4f          ;
977	122	94CA	          call d10s           ;
978			                              ;
979	124	54D7	          call sls0rf         ;
980	126	74D2	          call cls7rf         ;
981	128	94DE	          call d2s            ;
982	12A	7406	          call sls1rf         ;
983	12C	74E5	          call cls8rf         ;
984	12E	94DE	          call d2s            ;
985	130	7435	          call sls2rf         ;
986	132	9414	          call cls9rf         ;
987	134	94DE	          call d2s            ;
988	136	7464	          call sls3rf         ;
989	138	943E	          call cls10rf        ;
990	13A	748E	          call gcs4f          ;
991	13C	94CA	          call d10s           ;
992			                              ;
993	13E	54E0	          call sls0y          ;
994	140	F4DC	          call cls7y          ;
995	142	94DE	          call d2s            ;
996	144	740F	          call sls1y          ;
997	146	74EE	          call cls8y          ;
998	148	94DE	          call d2s            ;
999	14A	743E	          call sls2y          ;
1000	14C	F4F8	          call cls9y          ;
1001	14E	94DE	          call d2s            ;
1002	150	746D	          call sls3y          ;
1003	152	9455	          call cls10d         ;
1004	154	748E	          call gcs4f          ;
1005	156	94CA	          call d10s           ;
1006			                              ;
1007	158	54E5	          call sls0yf         ;
1008	15A	F4E1	          call cls7yf         ;
1009	15C	94DE	          call d2s            ;
1010	15E	7414	          call sls1yf         ;
1011	160	74F3	          call cls8yf         ;
1012	162	94DE	          call d2s            ;
1013	164	7443	          call sls2yf         ;
1014	166	941D	          call cls9yf         ;
1015	168	94DE	          call d2s            ;
1016	16A	7472	          call sls3yf         ;
1017	16C	748E	          call gcs4f          ;
1018	16E	94CA	          call d10s           ;
1019			                              ;
1020	170	54EE	          call sls0g          ;
1021	172	F4EA	          call cls7g          ;
1022	174	94DE	          call d2s            ;
1023	176	741D	          call sls1g          ;
1024	178	74FC	          call cls8g          ;
1025	17A	94DE	          call d2s            ;
1026	17C	744C	          call sls2g          ;
1027	17E	9426	          call cls9g          ;
1028	180	94DE	          call d2s            ;
1029	182	747B	          call sls3g          ;
1030	184	9447	          call cls10g         ;
1031	186	7497	          call gcs4d          ;
1032	188	94CA	          call d10s           ;
1033			                              ;
1034	18A	54D2	          call sls0r          ;
1035	18C	F4D7	          call cls7r          ;
1036	18E	94DE	          call d2s            ;
1037	190	7401	          call sls1r          ;
1038	192	74E0	          call cls8r          ;
1039	194	94DE	          call d2s            ;
1040	196	7430	          call sls2r          ;
1041	198	940F	          call cls9r          ;
1042	19A	94DE	          call d2s            ;
1043	19C	745F	          call sls3r          ;
1044	19E	9439	          call cls10r         ;
1045	1A0	94DE	          call d2s            ;
1046			                              ;
1047	1A2	B821	          mov  r0,#21         ; get mode loop counter
1048	1A4	F0	          mov  a,@r0          ; decrement
1049	1A5	07	          dec  a              ;
1050	1A6	A0	          mov  @r0,a          ; 
1051	1A7	C6AB	          jz   md0b           ;
1052	1A9	2410	          jmp  md0a           ;
1053	1AB	0400	md0b      jmp  reset          ;
1054			;
1055			;
1056			; ----------------------------
1057			;  mode 1
1058			; ----------------------------
1059			;
1060			;  m21 = mode loop counter
1061			;
1062	1AD	B821	md1       mov  r0,#21         ; set mode loop counter to 43
1063	1AF	B02B	          mov  @r0,#2b        ;   (82*43 = 1hour)
1064			                              ;
1065	1B1	54D2	md1a      call sls0r          ;
1066	1B3	F4D7	          call cls7r          ;
1067	1B5	740F	          call sls1y          ;
1068	1B7	74EE	          call cls8y          ;
1069	1B9	744C	          call sls2g          ;
1070	1BB	9426	          call cls9g          ;
1071	1BD	745F	          call sls3r          ;
1072	1BF	9439	          call cls10r         ;
1073	1C1	748E	          call gcs4f          ;
1074	1C3	94CA	          call d10s           ;
1075			                              ;
1076	1C5	54D7	          call sls0rf         ;
1077	1C7	74D2	          call cls7rf         ;
1078	1C9	94DE	          call d2s            ;
1079	1CB	7414	          call sls1yf         ;
1080	1CD	74F3	          call cls8yf         ;
1081	1CF	94DE	          call d2s            ;
1082	1D1	7430	          call sls2r          ;
1083	1D3	940F	          call cls9r          ;
1084	1D5	94DE	          call d2s            ;
1085	1D7	7464	          call sls3rf         ;
1086	1D9	748E	          call gcs4f          ;
1087	1DB	94CA	          call d10s           ;
1088			                              ;
1089	1DD	54E0	          call sls0y          ;
1090	1DF	F4DC	          call cls7y          ;
1091	1E1	94DE	          call d2s            ;
1092	1E3	741D	          call sls1g          ;
1093	1E5	74FC	          call cls8g          ;
1094	1E7	94DE	          call d2s            ;
1095	1E9	7435	          call sls2rf         ;
1096	1EB	9414	          call cls9rf         ;
1097	1ED	94DE	          call d2s            ;
1098	1EF	746D	          call sls3y          ;
1099	1F1	9455	          call cls10d         ;
1100	1F3	748E	          call gcs4f          ;
1101	1F5	94CA	          call d10s           ;
1102			                              ;
1103	1F7	54E5	          call sls0yf         ;
1104	1F9	F4E1	          call cls7yf         ;
1105	1FB	94DE	          call d2s            ;
1106	1FD	7401	          call sls1r          ;
1107	1FF	74E0	          call cls8r          ;
1108	201	94DE	          call d2s            ;
1109	203	743E	          call sls2y          ;
1110	205	F4F8	          call cls9y          ;
1111	207	94DE	          call d2s            ;
1112	209	7472	          call sls3yf         ;
1113	20B	9455	          call cls10d         ;
1114	20D	748E	          call gcs4f          ;
1115	20F	94CA	          call d10s           ;
1116			                              ;
1117	211	54EE	          call sls0g          ;
1118	213	F4EA	          call cls7g          ;     
1119	215	94DE	          call d2s            ;
1120	217	7406	          call sls1rf         ;
1121	219	74E5	          call cls8rf         ;
1122	21B	94DE	          call d2s            ;
1123	21D	7443	          call sls2yf         ;
1124	21F	941D	          call cls9yf         ;
1125	221	94DE	          call d2s            ;
1126	223	747B	          call sls3g          ;
1127	225	9447	          call cls10g         ;
1128	227	7497	          call gcs4d          ;
1129	229	94CA	          call d10s           ;
1130			                              ;
1131	22B	54D2	          call sls0r          ;
1132	22D	F4D7	          call cls7r          ;
1133	22F	94DE	          call d2s            ;
1134	231	740F	          call sls1y          ;
1135	233	74EE	          call cls8y          ;
1136	235	94DE	          call d2s            ;
1137	237	744C	          call sls2g          ;
1138	239	9426	          call cls9g          ;
1139	23B	94DE	          call d2s            ;          
1140	23D	745F	          call sls3r          ;
1141	23F	9439	          call cls10r         ;
1142	241	94DE	          call d2s            ;
1143			                              ;
1144	243	B821	          mov  r0,#21         ; get mode loop counter
1145	245	F0	          mov  a,@r0          ; decrement
1146	246	07	          dec  a              ;
1147	247	A0	          mov  @r0,a          ; 
1148	248	C64C	          jz   md1b           ;
1149	24A	24B1	          jmp  md1a           ;
1150	24C	0400	md1b      jmp  reset          ;
1151			;
1152			;
1153			; ----------------------------
1154			;  mode 2
1155			; ----------------------------
1156			;
1157	24E	54D2	md2       call sls0r          ;
1158	250	7401	          call sls1r          ;
1159	252	7430	          call sls2r          ;
1160	254	745F	          call sls3r          ;
1161	256	748E	          call gcs4f          ;
1162	258	F4D7	          call cls7r          ;
1163	25A	74E0	          call cls8r          ;
1164	25C	940F	          call cls9r          ;
1165	25E	9439	          call cls10r         ;
1166	260	44CE	          jmp  mdx            ;
1167			;
1168			;
1169			; ----------------------------
1170			;  mode 3
1171			; ----------------------------
1172			;
1173	262	54D7	md3       call sls0rf         ;
1174	264	7406	          call sls1rf         ;
1175	266	7435	          call sls2rf         ;
1176	268	7464	          call sls3rf         ;
1177	26A	748E	          call gcs4f          ;
1178	26C	74D2	          call cls7rf         ;
1179	26E	74E5	          call cls8rf         ;
1180	270	9414	          call cls9rf         ;
1181	272	943E	          call cls10rf        ;
1182	274	44CE	          jmp  mdx            ;
1183			;
1184			;
1185			; ----------------------------
1186			;  mode 4
1187			; ----------------------------
1188			;
1189	276	54E0	md4       call sls0y          ;
1190	278	740F	          call sls1y          ;
1191	27A	743E	          call sls2y          ;
1192	27C	746D	          call sls3y          ;
1193	27E	748E	          call gcs4f          ;
1194	280	F4DC	          call cls7y          ;
1195	282	74EE	          call cls8y          ;
1196	284	F4F8	          call cls9y          ;
1197	286	9455	          call cls10d         ;
1198	288	44CE	          jmp  mdx            ;
1199			;
1200			;
1201			; ----------------------------
1202			;  mode 5
1203			; ----------------------------
1204			;
1205	28A	54E5	md5       call sls0yf         ;
1206	28C	7414	          call sls1yf         ;
1207	28E	7443	          call sls2yf         ;
1208	290	7472	          call sls3yf         ;
1209	292	748E	          call gcs4f          ;
1210	294	F4E1	          call cls7yf         ;
1211	296	74F3	          call cls8yf         ;
1212	298	941D	          call cls9yf         ;
1213	29A	9455	          call cls10d         ;
1214	29C	44CE	          jmp  mdx            ;
1215			;
1216			;
1217			; ----------------------------
1218			;  mode 6
1219			; ----------------------------
1220			;
1221	29E	54EE	md6       call sls0g          ;
1222	2A0	741D	          call sls1g          ;
1223	2A2	744C	          call sls2g          ;
1224	2A4	747B	          call sls3g          ;
1225	2A6	748E	          call gcs4f          ;
1226	2A8	F4EA	          call cls7g          ;
1227	2AA	74FC	          call cls8g          ;
1228	2AC	9426	          call cls9g          ;
1229	2AE	9447	          call cls10g         ;
1230	2B0	44CE	          jmp  mdx            ;
1231			;
1232			;
1233			; ----------------------------
1234			;  mode 7
1235			; ----------------------------
1236			;
1237	2B2	54FC	md7       call sls0d          ;
1238	2B4	742B	          call sls1d          ;
1239	2B6	745A	          call sls2d          ;
1240	2B8	7489	          call sls3d          ;
1241	2BA	7497	          call gcs4d          ;
1242	2BC	74DB	          call cls7d          ;
1243	2BE	940A	          call cls8d          ;
1244	2C0	9434	          call cls9d          ;
1245	2C2	9455	          call cls10d         ;
1246	2C4	74B9	          call gy5mlc         ; turn off lights on gyralights 5 and 6
1247	2C6	74CD	          call gy6mlc         ;
1248	2C8	9475	          call ws11mlc        ; turn off lights on wig-wags 11 and 12
1249	2CA	9499	          call ws12mlc        ;
1250	2CC	44CE	          jmp  mdx            ;
1251			;
1252			;
1253			; ----------------------------
1254			;  mode x 
1255			; ----------------------------
1256			;
1257	2CE	94AC	mdx       call d1h            ;
1258	2D0	0400	          jmp  reset          ;
1259			;
1260			;
1261			; ----------------------------
1262			;  driver subroutines
1263			; ----------------------------
1264			;
1265			;    uses a, r0
1266			;
1267	2D2	B860	sls0r     mov  r0,#60         ;
1268	2D4	B012	          mov  @r0,#csls0r    ;
1269	2D6	83	          ret                 ;
1270			                              ;
1271	2D7	B860	sls0rf    mov  r0,#60         ;
1272	2D9	B0C3	          mov  @r0,#csls0rf   ;
1273	2DB	B870	          mov  r0,#70         ;
1274	2DD	B012	          mov  @r0,#csls0r    ;
1275	2DF	83	          ret                 ;
1276			                              ;
1277	2E0	B860	sls0y     mov  r0,#60         ;
1278	2E2	B014	          mov  @r0,#csls0y    ;
1279	2E4	83	          ret                 ;
1280			                              ;
1281	2E5	B860	sls0yf    mov  r0,#60         ;
1282	2E7	B0C7	          mov  @r0,#csls0yf   ;
1283	2E9	B870	          mov  r0,#70         ;
1284	2EB	B014	          mov  @r0,#csls0y    ;
1285	2ED	83	          ret                 ;
1286			                              ;
1287	2EE	B860	sls0g     mov  r0,#60         ;
1288	2F0	B019	          mov  @r0,#csls0g    ;
1289	2F2	83	          ret                 ;
1290			                              ;
1291	2F3	B860	sls0gf    mov  r0,#60         ;
1292	2F5	B0CB	          mov  @r0,#csls0gf   ;
1293	2F7	B870	          mov  r0,#70         ;
1294	2F9	B019	          mov  @r0,#csls0g    ;
1295	2FB	83	          ret                 ;
1296			                              ;
1297	2FC	B860	sls0d     mov  r0,#60         ;
1298	2FE	B013	          mov  @r0,#csls0rd   ;
1299	300	83	          ret                 ;
1300			                              ;
1301	301	B861	sls1r     mov  r0,#61         ;
1302	303	B022	          mov  @r0,#csls1r    ;
1303	305	83	          ret                 ;
1304			                              ;
1305	306	B861	sls1rf    mov  r0,#61         ;
1306	308	B0D3	          mov  @r0,#csls1rf   ;
1307	30A	B871	          mov  r0,#71         ;
1308	30C	B022	          mov  @r0,#csls1r    ;
1309	30E	83	          ret                 ;
1310			                              ;
1311	30F	B861	sls1y     mov  r0,#61         ;
1312	311	B024	          mov  @r0,#csls1y    ;
1313	313	83	          ret                 ;
1314			                              ;
1315	314	B861	sls1yf    mov  r0,#61         ;
1316	316	B0D7	          mov  @r0,#csls1yf   ;
1317	318	B871	          mov  r0,#71         ;
1318	31A	B024	          mov  @r0,#csls1y    ;
1319	31C	83	          ret                 ;
1320			                              ;
1321	31D	B861	sls1g     mov  r0,#61         ;
1322	31F	B029	          mov  @r0,#csls1g    ;
1323	321	83	          ret                 ;
1324			                              ;
1325	322	B861	sls1gf    mov  r0,#61         ;
1326	324	B0DB	          mov  @r0,#csls1gf   ;
1327	326	B871	          mov  r0,#71         ;
1328	328	B029	          mov  @r0,#csls1g    ;
1329	32A	83	          ret                 ;
1330			                              ;
1331	32B	B861	sls1d     mov  r0,#61         ;
1332	32D	B023	          mov  @r0,#csls1rd   ;
1333	32F	83	          ret                 ;
1334			                              ;
1335	330	B862	sls2r     mov  r0,#62         ;
1336	332	B042	          mov  @r0,#csls2r    ;
1337	334	83	          ret                 ;
1338			                              ;
1339	335	B862	sls2rf    mov  r0,#62         ;
1340	337	B093	          mov  @r0,#csls2rf   ;
1341	339	B872	          mov  r0,#72         ;
1342	33B	B042	          mov  @r0,#csls2r    ;
1343	33D	83	          ret                 ;
1344			                              ;
1345	33E	B862	sls2y     mov  r0,#62         ;
1346	340	B044	          mov  @r0,#csls2y    ;
1347	342	83	          ret                 ;
1348			                              ;
1349	343	B862	sls2yf    mov  r0,#62         ;
1350	345	B097	          mov  @r0,#csls2yf   ;
1351	347	B872	          mov  r0,#72         ;
1352	349	B044	          mov  @r0,#csls2y    ;
1353	34B	83	          ret                 ;
1354			                              ;
1355	34C	B862	sls2g     mov  r0,#62         ;
1356	34E	B049	          mov  @r0,#csls2g    ;
1357	350	83	          ret                 ;
1358			                              ;
1359	351	B862	sls2gf    mov  r0,#62         ;
1360	353	B09B	          mov  @r0,#csls2gf   ;
1361	355	B872	          mov  r0,#72         ;
1362	357	B049	          mov  @r0,#csls2g    ;
1363	359	83	          ret                 ;
1364			                              ;
1365	35A	B862	sls2d     mov  r0,#62         ;
1366	35C	B043	          mov  @r0,#csls2rd   ;
1367	35E	83	          ret                 ;
1368			                              ;
1369	35F	B863	sls3r     mov  r0,#63         ;
1370	361	B042	          mov  @r0,#csls2r    ;
1371	363	83	          ret                 ;
1372			                              ;
1373	364	B863	sls3rf    mov  r0,#63         ;
1374	366	B0B3	          mov  @r0,#csls3rf   ;
1375	368	B873	          mov  r0,#73         ;
1376	36A	B002	          mov  @r0,#csls3r    ;
1377	36C	83	          ret                 ;
1378			                              ;
1379	36D	B863	sls3y     mov  r0,#63         ;
1380	36F	B004	          mov  @r0,#csls3y    ;
1381	371	83	          ret                 ;
1382			                              ;
1383	372	B863	sls3yf    mov  r0,#63         ;
1384	374	B0B7	          mov  @r0,#csls3yf   ;
1385	376	B873	          mov  r0,#73         ;
1386	378	B004	          mov  @r0,#csls3y    ;
1387	37A	83	          ret                 ;
1388			                              ;
1389	37B	B863	sls3g     mov  r0,#63         ;
1390	37D	B009	          mov  @r0,#csls3g    ;
1391	37F	83	          ret                 ;
1392			                              ;
1393	380	B863	sls3gf    mov  r0,#63         ;
1394	382	B0BB	          mov  @r0,#csls3gf   ;
1395	384	B872	          mov  r0,#72         ;
1396	386	B009	          mov  @r0,#csls3g    ;
1397	388	83	          ret                 ;
1398			                              ;
1399	389	B863	sls3d     mov  r0,#63         ;
1400	38B	B003	          mov  @r0,#csls3rd   ;
1401	38D	83	          ret                 ;
1402			                              ;
1403	38E	B864	gcs4f     mov  r0,#64         ;
1404	390	B086	          mov  @r0,#cgcs4l    ; 
1405	392	B874	          mov  r0,#74         ;
1406	394	B035	          mov  @r0,#cgcs4r    ; 
1407	396	83	          ret                 ;
1408			                              ;
1409	397	B864	gcs4d     mov  r0,#64         ;
1410	399	B007	          mov  @r0,#cgcs4d    ; 
1411	39B	83	          ret                 ;
1412			                              ;
1413	39C	B864	gcs4fb    mov  r0,#64         ;
1414	39E	B082	          mov  @r0,#cgcs4lb   ; 
1415	3A0	B874	          mov  r0,#74         ;
1416	3A2	B031	          mov  @r0,#cgcs4rb   ; 
1417	3A4	83	          ret                 ;
1418			                              ;
1419	3A5	B864	gcs4db    mov  r0,#64         ;
1420	3A7	B033	          mov  @r0,#cgcs4db   ; 
1421	3A9	83	          ret                 ;
1422			                              ;
1423	3AA	B865	gy5mo     mov  r0,#65         ;
1424	3AC	B001	          mov  @r0,#cgy5mo    ; 
1425	3AE	83	          ret                 ;
1426			                              ;
1427	3AF	B865	gy5lo     mov  r0,#65         ;
1428	3B1	B002	          mov  @r0,#cgy5lo    ; 
1429	3B3	83	          ret                 ;
1430			                              ;
1431	3B4	B865	gy5mlo    mov  r0,#65         ;
1432	3B6	B003	          mov  @r0,#cgy5mlo   ; 
1433	3B8	83	          ret                 ;
1434			                              ;
1435	3B9	B865	gy5mlc    mov  r0,#65         ;
1436	3BB	B000	          mov  @r0,#cgy5mlc   ; 
1437	3BD	83	          ret                 ;
1438			                              ;
1439	3BE	B866	gy6mo     mov  r0,#66         ;
1440	3C0	B001	          mov  @r0,#cgy6mo    ; 
1441	3C2	83	          ret                 ;
1442			                              ;
1443	3C3	B866	gy6lo     mov  r0,#66         ;
1444	3C5	B002	          mov  @r0,#cgy6lo    ; 
1445	3C7	83	          ret                 ;
1446			                              ;
1447	3C8	B866	gy6mlo    mov  r0,#66         ;
1448	3CA	B003	          mov  @r0,#cgy6mlo   ; 
1449	3CC	83	          ret                 ;
1450			                              ;
1451	3CD	B866	gy6mlc    mov  r0,#66         ;
1452	3CF	B000	          mov  @r0,#cgy6mlc   ; 
1453	3D1	83	          ret                 ;
1454			                              ;
1455	3D2	B867	cls7rf    mov  r0,#67         ;
1456	3D4	B0BF	          mov  @r0,#ccls7rf   ;
1457	3D6	B877	          mov  r0,#77         ;
1458	3D8	B00E	          mov  @r0,#ccls7r    ;
1459	3DA	83	          ret                 ;
1460			                              ;
1461	3DB	B867	cls7d     mov  r0,#67         ; some driver routines moved to end
1462	3DD	B00F	          mov  @r0,#ccls7rd   ;   moved to end to make space on 
1463	3DF	83	          ret                 ;   on this memory page
1464			                              ;
1465	3E0	B868	cls8r     mov  r0,#68         ;
1466	3E2	B01E	          mov  @r0,#ccls8r    ;
1467	3E4	83	          ret                 ;
1468			                              ;
1469	3E5	B868	cls8rf    mov  r0,#68         ;
1470	3E7	B0CF	          mov  @r0,#ccls8rf   ;
1471	3E9	B878	          mov  r0,#78         ;
1472	3EB	B01E	          mov  @r0,#ccls8r    ;
1473	3ED	83	          ret                 ;
1474			                              ;
1475	3EE	B868	cls8y     mov  r0,#68         ;
1476	3F0	B01D	          mov  @r0,#ccls8y    ;
1477	3F2	83	          ret                 ;
1478			                              ;
1479	3F3	B868	cls8yf    mov  r0,#68         ;
1480	3F5	B0CF	          mov  @r0,#ccls8yf   ;
1481	3F7	B878	          mov  r0,#78         ;
1482	3F9	B01D	          mov  @r0,#ccls8y    ;
1483	3FB	83	          ret                 ;
1484			                              ;
1485	3FC	B868	cls8g     mov  r0,#68         ;
1486	3FE	B01B	          mov  @r0,#ccls8g    ;
1487	400	83	          ret                 ;
1488			                              ;
1489	401	B868	cls8gf    mov  r0,#68         ;
1490	403	B0CF	          mov  @r0,#ccls8gf   ;
1491	405	B878	          mov  r0,#78         ;
1492	407	B01B	          mov  @r0,#ccls8g    ;
1493	409	83	          ret                 ;
1494			                              ;
1495	40A	B868	cls8d     mov  r0,#68         ;
1496	40C	B01F	          mov  @r0,#ccls8rd   ;
1497	40E	83	          ret                 ;
1498			                              ;
1499	40F	B869	cls9r     mov  r0,#69         ;
1500	411	B02E	          mov  @r0,#ccls9r    ;
1501	413	83	          ret                 ;
1502			                              ;
1503	414	B869	cls9rf    mov  r0,#69         ;
1504	416	B0DF	          mov  @r0,#ccls9rf   ;
1505	418	B879	          mov  r0,#79         ;
1506	41A	B02E	          mov  @r0,#ccls9r    ;
1507	41C	83	          ret                 ;
1508			                              ;
1509	41D	B869	cls9yf    mov  r0,#69         ;
1510	41F	B0DF	          mov  @r0,#ccls9yf   ;
1511	421	B879	          mov  r0,#79         ;
1512	423	B02D	          mov  @r0,#ccls9y    ;
1513	425	83	          ret                 ;
1514			                              ;
1515	426	B869	cls9g     mov  r0,#69         ;
1516	428	B02B	          mov  @r0,#ccls9g    ;
1517	42A	83	          ret                 ;
1518			                              ;
1519	42B	B869	cls9gf    mov  r0,#69         ;
1520	42D	B0DF	          mov  @r0,#ccls9gf   ;
1521	42F	B879	          mov  r0,#79         ;
1522	431	B02B	          mov  @r0,#ccls9g    ;
1523	433	83	          ret                 ;
1524			                              ;
1525	434	B869	cls9d     mov  r0,#69         ;
1526	436	B02F	          mov  @r0,#ccls9rd   ;
1527	438	83	          ret                 ;
1528			                              ;
1529	439	B86A	cls10r    mov  r0,#6a         ;
1530	43B	B01E	          mov  @r0,#ccls10r   ;
1531	43D	83	          ret                 ;
1532			                              ;
1533	43E	B86A	cls10rf   mov  r0,#6a         ;
1534	440	B0CF	          mov  @r0,#ccls10rf  ;
1535	442	B87A	          mov  r0,#7a         ;
1536	444	B01E	          mov  @r0,#ccls10r   ;
1537	446	83	          ret                 ;
1538			                              ;
1539	447	B86A	cls10g    mov  r0,#6a         ;
1540	449	B01D	          mov  @r0,#ccls10g   ;
1541	44B	83	          ret                 ;
1542			                              ;
1543	44C	B86A	cls10gf   mov  r0,#6a         ;
1544	44E	B0CF	          mov  @r0,#ccls10gf  ;
1545	450	B87A	          mov  r0,#7a         ;
1546	452	B01D	          mov  @r0,#ccls10g   ;
1547	454	83	          ret                 ;
1548			                              ;
1549	455	B86A	cls10d    mov  r0,#6a         ;
1550	457	B01F	          mov  @r0,#ccls10rd  ;
1551	459	83	          ret                 ;
1552			                              ;               
1553	45A	B86B	ws11mo    mov  r0,#6b         ; get current instruction
1554	45C	F0	          mov  a,@r0          ;
1555	45D	53FC	          anl  a,#fc          ; strip lower bits 
1556	45F	D301	          xrl  a,#cws11mo     ; add current instruction
1557	461	A0	          mov  @r0,a          ; 
1558	462	83	          ret                 ;
1559			                              ;
1560	463	B86B	ws11lo    mov  r0,#6b         ; get current instruction
1561	465	F0	          mov  a,@r0          ;
1562	466	53FC	          anl  a,#fc          ; strip lower bits 
1563	468	D302	          xrl  a,#cws11lo     ; add current instruction
1564	46A	A0	          mov  @r0,a          ; 
1565	46B	83	          ret                 ;
1566			                              ;
1567	46C	B86B	ws11mlo   mov  r0,#6b         ; get current instruction
1568	46E	F0	          mov  a,@r0          ;
1569	46F	53FC	          anl  a,#fc          ; strip lower bits 
1570	471	D303	          xrl  a,#cws11mlo    ; add current instruction
1571	473	A0	          mov  @r0,a          ; 
1572	474	83	          ret                 ;
1573			                              ;
1574	475	B86B	ws11mlc   mov  r0,#6b         ; get current instruction
1575	477	F0	          mov  a,@r0          ;
1576	478	53FC	          anl  a,#fc          ; strip lower bits 
1577	47A	D300	          xrl  a,#cws11mlc    ; add current instruction
1578	47C	A0	          mov  @r0,a          ; 
1579	47D	83	          ret                 ;
1580			                              ;
1581	47E	B86B	ws12mo    mov  r0,#6b         ; get current instruction
1582	480	F0	          mov  a,@r0          ;
1583	481	53F3	          anl  a,#f3          ; strip upper bits 
1584	483	D304	          xrl  a,#cws12mo     ; add current instruction
1585	485	A0	          mov  @r0,a          ; 
1586	486	83	          ret                 ;
1587			                              ;
1588	487	B86B	ws12lo    mov  r0,#6b         ; get current instruction
1589	489	F0	          mov  a,@r0          ;
1590	48A	53F3	          anl  a,#f3          ; strip upper bits 
1591	48C	D308	          xrl  a,#cws12lo     ; add current instruction
1592	48E	A0	          mov  @r0,a          ; 
1593	48F	83	          ret                 ;
1594			                              ;
1595	490	B86B	ws12mlo   mov  r0,#6b         ; get current instruction
1596	492	F0	          mov  a,@r0          ;
1597	493	53F3	          anl  a,#f3          ; strip upper bits 
1598	495	D30C	          xrl  a,#cws12mlo    ; add current instruction
1599	497	A0	          mov  @r0,a          ; 
1600	498	83	          ret                 ;
1601			                              ;
1602	499	B86B	ws12mlc   mov  r0,#6b         ; get current instruction
1603	49B	F0	          mov  a,@r0          ;
1604	49C	53F3	          anl  a,#f3          ; strip upper bits 
1605	49E	D300	          xrl  a,#cws12mlc    ; add current instruction
1606	4A0	A0	          mov  @r0,a          ; 
1607	4A1	83	          ret                 ;
1608			;
1609			;
1610			; ----------------------------
1611			;  dx subroutine
1612			; ----------------------------
1613			;
1614			;  m22 = delay loop counter 0 - 1 to 256 seconds 
1615			;  m23 = delay loop counter 1 - 4 minutes to 18 hours
1616			;
1617			;    1 second to 18 hours
1618			;    1 second per count
1619			;    uses a and r0
1620			;    subroutine calls use all registers
1621			;
1622	4A2	B823	d4h       mov  r0,#23         ; load counters
1623	4A4	B038	          mov  @r0,#38        ;
1624	4A6	B822	          mov  r0,#22         ; 
1625	4A8	B040	          mov  @r0,#40        ;
1626	4AA	84E8	          jmp  dx0            ;
1627	4AC	B823	d1h       mov  r0,#23         ; load counters
1628	4AE	B00E	          mov  @r0,#0e        ;
1629	4B0	B822	          mov  r0,#22         ; 
1630	4B2	B010	          mov  @r0,#10        ;
1631	4B4	84E8	          jmp  dx0            ;
1632	4B6	B823	d1m       mov  r0,#23         ; load counters
1633	4B8	B000	          mov  @r0,#00        ;
1634	4BA	B822	          mov  r0,#22         ; 
1635	4BC	B03C	          mov  @r0,#3c        ;
1636	4BE	84E8	          jmp  dx0            ;
1637	4C0	B823	d30s      mov  r0,#23         ; load counters
1638	4C2	B000	          mov  @r0,#00        ;
1639	4C4	B822	          mov  r0,#22         ;
1640	4C6	B01E	          mov  @r0,#1e        ;
1641	4C8	84E8	          jmp  dx0            ;
1642	4CA	B823	d10s      mov  r0,#23         ; load counters
1643	4CC	B000	          mov  @r0,#00        ;
1644	4CE	B822	          mov  r0,#22         ;
1645	4D0	B00A	          mov  @r0,#0a        ;
1646	4D2	84E8	          jmp  dx0            ;
1647	4D4	B823	d5s       mov  r0,#23         ; load counters
1648	4D6	B000	          mov  @r0,#00        ;
1649	4D8	B822	          mov  r0,#22         ;
1650	4DA	B005	          mov  @r0,#05        ;
1651	4DC	84E8	          jmp  dx0            ;
1652	4DE	B823	d2s       mov  r0,#23         ; load counters
1653	4E0	B000	          mov  @r0,#00        ;
1654	4E2	B822	          mov  r0,#22         ;
1655	4E4	B002	          mov  @r0,#02        ;
1656	4E6	84E8	          jmp  dx0            ;
1657			                              ;
1658	4E8	2300	dx0       mov  a,#00          ; phase 0
1659	4EA	F40B	          call update         ;
1660	4EC	2310	          mov  a,#10          ; phase 1
1661	4EE	F40B	          call update         ;
1662	4F0	2320	          mov  a,#20          ; phase 2
1663	4F2	F40B	          call update         ;
1664	4F4	2330	          mov  a,#30          ; phase 3 
1665	4F6	F40B	          call update         ;
1666	4F8	2340	          mov  a,#40          ; phase 4 
1667	4FA	F40B	          call update         ;
1668	4FC	2350	          mov  a,#50          ; phase 5 
1669	4FE	F40B	          call update         ;
1670			                              ;
1671	500	B822	          mov  r0,#22         ; loop until delay complete
1672	502	F0	          mov  a,@r0          ; decrement lsb
1673	503	07	          dec  a              ;
1674	504	A0	          mov  @r0,a          ;
1675	505	D3FF	          xrl  a,#ff          ; see if borrow occurred
1676	507	960E	          jnz  dx1            ; 
1677	509	B823	          mov  r0,#23         ; if borrow, decrement msb
1678	50B	F0	          mov  a,@r0          ;
1679	50C	07	          dec  a              ; 
1680	50D	A0	          mov  @r0,a          ;
1681	50E	B822	dx1       mov  r0,#22         ;
1682	510	F0	          mov  a,@r0          ; test if zero
1683	511	18	          inc  r0             ;
1684	512	40	          orl  a,@r0          ;
1685	513	C617	          jz   dx2            ;
1686	515	84E8	          jmp  dx0            ;
1687	517	83	dx2       ret                 ; 
1688			;          
1689			;
1690			; ----------------------------
1691			;  input subroutine
1692			; ----------------------------
1693			;
1694			;  checks inputs and sets mode based on inputs
1695			;    input state register keeps track of state
1696			;    next state depends on current state, counters and input values
1697			;
1698			;  no result is returned
1699			;
1700			;  variables
1701			;    m20 = mode
1702			;    m21 = mode cycle count
1703			;    m28 = input state - 0 = nibble 1 
1704			;                        1 = digit one no input 
1705			;                        2 = nibble 2 timeout
1706			;                        3 = digit two no input
1707			;                        4 = switch
1708			;    m29 = input data register - msn, lsn
1709			;    m2a = input counter 
1710			;    m2b = red led state
1711			;
1712			;    m2e = remote msb data
1713			;    m2f = remote lsb data
1714			;
1715			;  f0 set for switch input
1716			;  f1 set for valid remote input
1717			;
1718			;  uses a, r0, r1
1719			;    calls use a, r0 and r2-r7
1720			;
1721			;
1722	518	85	input     clr  f0             ; clear flags
1723	519	A5	          clr  f1             ;
1724			                              ;
1725	51A	27	          clr  a              ; clear remote data
1726	51B	B82E	          mov  r0,#2e         ;
1727	51D	A0	          mov  @r0,a          ;
1728	51E	B82F	          mov  r0,#2f         ;
1729	520	A0	          mov  @r0,a          ;
1730			                              ;
1731			                              ; check inputs two times
1732			                              ;   in 1/6 (0.167) second delay
1733	521	D405	          call valrem         ; check if valid remote input
1734	523	D400	          call switch         ; check if local input
1735	525	D405	          call valrem         ; 
1736	527	D400	          call switch         ; 
1737			                              ;
1738	529	B82B	          mov  r0,#2b         ; get red led status
1739	52B	F0	          mov  a,@r0          ;               
1740	52C	C634	          jz   inputa         ;
1741	52E	9ABF	          anl  p2,#bf         ; turn off red led
1742	530	2300	          mov  a,#00          ; set flag as off
1743	532	A438	          jmp  inputb         ;
1744	534	8A40	inputa    orl  p2,#40         ; turn on red led
1745	536	2301	          mov  a,#01          ; set flag as on
1746	538	A0	inputb    mov  @r0,a          ; update flag
1747			                              ;
1748	539	B6C4	          jf0  input4         ; check if switch input
1749			                              ;
1750	53B	B828	          mov  r0,#28         ; check if state 0 - nibble 1
1751	53D	F0	          mov  a,@r0          ;
1752	53E	D300	          xrl  a,#00          ;
1753	540	965B	          jnz  input1         ;
1754	542	7645	          jf1  input0         ; check if valid input
1755	544	83	          ret                 ;
1756	545	B82F	input0    mov  r0,#2f         ; get lsb data
1757	547	F0	          mov  a,@r0          ;
1758	548	17	          inc  a              ; adjust lsb for remote coding
1759	549	530F	          anl  a,#0f          ;
1760	54B	A9	          mov  r1,a           ;
1761	54C	D30A	          xrl  a,#0a          ; 0 = 1 ... 8 = 9, 9 = 0
1762	54E	9652	          jnz  input0a        ;
1763	550	B900	          mov  r1,#00         ;
1764	552	F9	input0a   mov  a,r1           ; get adjusted value
1765	553	B829	          mov  r0,#29         ; store value in input register - lsn
1766	555	A0	          mov  @r0,a          ;
1767	556	B828	          mov  r0,#28         ; change state to 1
1768	558	B001	          mov  @r0,#01        ;
1769	55A	83	          ret                 ;
1770			                              ;
1771	55B	B828	input1    mov  r0,#28         ; check if state 1 - digit one no input
1772	55D	F0	          mov  a,@r0          ;
1773	55E	D301	          xrl  a,#01          ;
1774	560	966D	          jnz  input2         ;
1775	562	766C	          jf1  input1a        ; test for no valid input
1776	564	B82A	          mov  r0,#2a         ; set counter 0 to 6
1777	566	B006	          mov  @r0,#06        ;   (6 samples * .25 for 1.5 seconds)
1778	568	B828	          mov  r0,#28         ; change state to 2
1779	56A	B002	          mov  @r0,#02        ;
1780	56C	83	input1a   ret                 ;                              
1781			                              ;
1782	56D	B828	input2    mov  r0,#28         ; check if state 2 - nibble 2 timeout
1783	56F	F0	          mov  a,@r0          ;
1784	570	D302	          xrl  a,#02          ;
1785	572	96A7	          jnz  input3         ;
1786	574	768B	          jf1  input2a        ;
1787	576	B82A	          mov  r0,#2a         ; decrement counter
1788	578	F0	          mov  a,@r0          ; 
1789	579	07	          dec  a              ;
1790	57A	A0	          mov  @r0,a          ;
1791	57B	96A6	          jnz  input2c        ; if 0, only one digit
1792	57D	B829	          mov  r0,#29         ; get lsn data
1793	57F	F0	          mov  a,@r0          ;
1794	580	B820	          mov  r0,#20         ; store lsn data as new mode
1795	582	A0	          mov  @r0,a          ;
1796	583	B828	          mov  r0,#28         ; set state to 0
1797	585	27	          clr  a              ;
1798	586	A0	          mov  @r0,a          ;
1799	587	27	          clr  a              ; reset stack pointer and program status word 
1800	588	D7	          mov  psw,a          ;   CY, AC, F0, BS, 1, SP2, SP1, SP0   
1801	589	0436	          jmp  start          ;
1802	58B	B82F	input2a   mov  r0,#2f         ; get lsb data
1803	58D	F0	          mov  a,@r0          ;
1804	58E	17	          inc  a              ; adjust lsb for remote coding
1805	58F	530F	          anl  a,#0f          ;
1806	591	A9	          mov  r1,a           ;
1807	592	D30A	          xrl  a,#0a          ; 0 = 1 ... 8 = 9, 9 = 0
1808	594	9698	          jnz  input2b        ;
1809	596	B900	          mov  r1,#00         ;
1810	598	B829	input2b   mov  r0,#29         ; get former lsn from input register
1811	59A	F0	          mov  a,@r0          ;
1812	59B	E7	          rl   a              ; multiply former lsn by 10(0x0a) to get msn
1813	59C	AA	          mov  r2,a           ;
1814	59D	E7	          rl   a              ;
1815	59E	E7	          rl   a              ;
1816	59F	6A	          add  a,r2           ;
1817	5A0	69	          add  a,r1           ; add new lsn
1818	5A1	A0	          mov  @r0,a          ; store result
1819	5A2	B828	          mov  r0,#28         ; set state to 3
1820	5A4	B003	          mov  @r0,#03        ;
1821	5A6	83	input2c   ret                 ;
1822			                              ;
1823	5A7	B828	input3    mov  r0,#28         ; check if state 3 - digit two no input
1824	5A9	F0	          mov  a,@r0          ;
1825	5AA	D303	          xrl  a,#03          ;
1826	5AC	96C4	          jnz  input4         ;
1827	5AE	76C3	          jf1  input3a        ; test for no valid input
1828	5B0	B828	          mov  r0,#28         ; set state to 0
1829	5B2	27	          clr  a              ;
1830	5B3	A0	          mov  @r0,a          ;
1831	5B4	B829	          mov  r0,#29         ; get result from input register
1832	5B6	F0	          mov  a,@r0          ;
1833	5B7	03AD	          add  a,#cmmc        ; test if over max mode
1834	5B9	F6C3	          jc   input3a        ;
1835	5BB	F0	          mov  a,@r0          ; get result from input register again
1836	5BC	B820	          mov  r0,#20         ; store result as new mode
1837	5BE	A0	          mov  @r0,a          ;
1838	5BF	27	          clr  a              ; reset stack pointer and program status word 
1839	5C0	D7	          mov  psw,a          ;   CY, AC, F0, BS, 1, SP2, SP1, SP0  
1840	5C1	0436	          jmp  start          ;
1841	5C3	83	input3a   ret                 ;
1842			                              ;
1843	5C4	B828	input4    mov  r0,#28         ; state 4 - switch
1844	5C6	F0	          mov  a,@r0          ;
1845	5C7	D304	          xrl  a,#04          ; 
1846	5C9	C6D2	          jz   input4a        ;
1847	5CB	B004	          mov  @r0,#04        ; if status not 4, set to 4
1848	5CD	B82A	          mov  r0,#2a         ; set power off counter
1849	5CF	B009	          mov  @r0,#09        ;   to 9 (1.5 seconds)
1850	5D1	83	          ret                 ;
1851	5D2	B6E9	input4a   jf0  input4c        ; if status 4, wait for no switch
1852	5D4	B820	          mov  r0,#20         ; get mode
1853	5D6	F0	          mov  a,@r0          ;
1854	5D7	17	          inc  a              ; 
1855	5D8	A9	          mov  r1,a           ;
1856	5D9	03AD	          add  a,#cmmc        ; test if over maximum mode
1857	5DB	E6DF	          jnc  input4b        ;
1858	5DD	B900	          mov  r1,#00         ;
1859	5DF	F9	input4b   mov  a,r1           ; store mode
1860	5E0	A0	          mov  @r0,a          ;
1861	5E1	B828	          mov  r0,#28         ; set state to 0
1862	5E3	27	          clr  a              ;
1863	5E4	A0	          mov  @r0,a          ;
1864	5E5	27	          clr  a              ; reset stack pointer and program status word 
1865	5E6	D7	          mov  psw,a          ;   CY, AC, F0, BS, 1, SP2, SP1, SP0  
1866	5E7	0436	          jmp  start          ;
1867	5E9	B82A	input4c   mov  r0,#2a         ; decrement counter
1868	5EB	F0	          mov  a,@r0          ; 
1869	5EC	07	          dec  a              ;
1870	5ED	A0	          mov  @r0,a          ;
1871	5EE	96F2	          jnz  input4d        ; power off if count is 0  
1872	5F0	0400	          jmp  reset          ; 
1873	5F2	83	input4d   ret                 ;
1874			;
1875			;
1876			          neworg 600          ;
1876	5F3	00	;
1876	5F4	00	;
1876	5F5	00	;
1876	5F6	00	;
1876	5F7	00	;
1876	5F8	00	;
1876	5F9	00	;
1876	5FA	00	;
1876	5FB	00	;
1876	5FC	00	;
1876	5FD	00	;
1876	5FE	00	;
1876	5FF	00	;
1877			;
1878			;
1879			; ----------------------------
1880			;  switch routine
1881			; ----------------------------
1882			;
1883			;  sets f0 if switch input - active low
1884			;
1885			;  switch input = t0 (pin 1) active low
1886			;
1887			;
1888	600	3604	switch    jt0  switch1        ; test switch
1889	602	85	          clr  f0             ; if switch input, set f0
1890	603	95	          cpl  f0             ;
1891	604	83	switch1   ret                 ;
1892			;
1893			;
1894			; ----------------------------
1895			;  valrem routine
1896			; ----------------------------
1897			;
1898			;  sets f1 if valid remote data found
1899			;
1900			;  stores valid msb data in m2e
1901			;  stores valid lsb data in m2f
1902			;
1903			;  uses a, r0
1904			;    calls use a, r0 and r2-r7
1905			;
1906			;
1907	605	D425	valrem    call rem
1908	607	FB	          mov  a,r3           ; get msb data
1909	608	D305	          xrl  a,#05          ; check msb data
1910	60A	9624	          jnz  valrem1        ; 
1911	60C	FA	          mov  a,r2           ; get lsb data
1912	60D	53F0	          anl  a,#f0          ;
1913	60F	D380	          xrl  a,#80          ; check lsb msn data
1914	611	9624	          jnz  valrem1        ; 
1915	613	FA	          mov  a,r2           ; get lsb data
1916	614	530F	          anl  a,#0f          ;
1917	616	03F6	          add  a,#f6          ; check lsb lsn data <= 9
1918	618	F624	          jc   valrem1        ; 
1919	61A	A5	          clr  f1             ; if data valid, set f1
1920	61B	B5	          cpl  f1             ;
1921	61C	FB	          mov  a,r3           ; get msb data
1922	61D	B82E	          mov  r0,#2e         ; 
1923	61F	A0	          mov  @r0,a          ; store data 
1924	620	FA	          mov  a,r2           ; get lsb data
1925	621	B82F	          mov  r0,#2f         ;
1926	623	A0	          mov  @r0,a          ; store data 
1927	624	83	valrem1   ret                 ;
1928			;
1929			;
1930			; ----------------------------
1931			;  rem routine
1932			; ----------------------------
1933			;
1934			;    if data, 0x01 returned in a
1935			;    if no data, 0x00 returned in a 
1936			;
1937			;    remote msb data returned in r3
1938			;    remote lsb data returned in r2 
1939			;
1940			;    registers
1941			;    r0 - count data index register 
1942			;    r1 - 
1943			
1944			;    r2 - lsb data
1945			;    r3 - msb data
1946			;    r4 - lsb aggregate count 
1947			;    r5 - msb aggregate count
1948			;    r6 - count
1949			;    r7 - loop count
1950			;
1951			;    remote input stream on t1
1952			;
1953	625	27	rem       clr  a              ;
1954	626	AB	          mov  r3,a           ; clear msb data
1955	627	AA	          mov  r2,a           ; clear lsb data
1956			
1957	628	AC	          mov  r4,a           ; clear lsb aggregate count
1958	629	AD	          mov  r5,a           ; clear msb aggregate count
1959			                              ;
1960	62A	AF	          mov  r7,a           ; clear loop count
1961			                              ;
1962			                              ; one sync
1963	62B	B832	rem1      mov  r0,#32         ; set up count data pointer 
1964			                              ;   for test
1965	62D	BE00	          mov  r6,#00         ; clear count
1966			                              ;
1967	62F	F400	rem2      call delayr         ; 100 us delay between counts
1968			                              ;
1969	631	FC	          mov  a,r4           ; increment aggregate count
1970	632	0301	          add  a,#01          ;
1971	634	AC	          mov  r4,a           ;
1972	635	FD	          mov  a,r5           ;
1973	636	1300	          addc a,#00          ;
1974	638	AD	          mov  r5,a           ;
1975			                              ;
1976	639	FD	          mov  a,r5           ; check aggregate count
1977	63A	03FD	          add  a,#fd          ; if over 3 * 256 = 768
1978			                              ;   3 generates a carry
1979	63C	F6F6	          jc   rem12          ; finish if maximum count reached
1980			                              ;
1981	63E	FE	          mov  a,r6           ; increment count
1982	63F	0301	          add  a,#01          ;
1983	641	E645	          jnc  rem3           ; limit overflow
1984	643	23FF	          mov  a,#ff          ;
1985	645	AE	rem3      mov  r6,a           ;
1986			                              ;
1987	646	00	          nop                 ; input data
1988	647	00	          nop                 ;
1989	648	562F	          jt1  rem2           ; if one, get one data
1990			                              ;
1991	64A	FE	          mov  a,r6           ; if zero, check if one low count 
1992			                              ;   at least ** 160 ** (#a0)
1993	64B	0360	          add  a,#60          ; 160 (#a0) generates a carry
1994	64D	E62B	          jnc  rem1           ; if not greater, restart one sync
1995			                              ;
1996	64F	FE	          mov  a,r6           ; get count
1997	650	A0	          mov  @r0,a          ; store count data for test
1998	651	18	          inc  r0             ; increment pointer
1999			                              ;
2000			                              ; zero sync
2001	652	BE00	          mov  r6,#00         ; clear count
2002			                              ;
2003	654	F400	rem4      call delayr         ; 100 us delay between counts
2004			                              ;
2005	656	FC	          mov  a,r4           ; increment aggregate count
2006	657	0301	          add  a,#01          ;
2007	659	AC	          mov  r4,a           ;
2008	65A	FD	          mov  a,r5           ;
2009	65B	1300	          addc a,#00          ;
2010	65D	AD	          mov  r5,a           ;
2011			                              ;
2012	65E	FD	          mov  a,r5           ; check aggregate count
2013	65F	03FD	          add  a,#fd          ; if over 3 * 256 = 768
2014			                              ;   3 generates a carry
2015	661	F6F6	          jc   rem12          ; finish if maximum count reached
2016			                              ;
2017	663	FE	          mov  a,r6           ; increment count
2018	664	17	          inc  a              ;
2019	665	AE	          mov  r6,a           ;
2020			                              ;     
2021	666	00	          nop                 ; input data
2022	667	00	          nop                 ;
2023	668	4654	          jnt1 rem4           ; if zero, get zero data
2024			                              ;
2025	66A	FE	          mov  a,r6           ; if one, check if zero low count
2026			                              ;   at least ** 20 ** (#14)
2027	66B	03EC	          add  a,#ec          ; 20 (#14) generates a carry
2028	66D	E62B	          jnc  rem1           ; if not greater, restart one sync
2029			                              ;
2030	66F	FE	          mov  a,r6           ; if one, check if zero high count
2031			                              ;   no more than ** 32 ** (#20)
2032	670	03DF	          add  a,#df          ; 33 (#21) generates a carry
2033	672	F62B	          jc   rem1           ; if not less, restart one sync
2034			                              ;
2035	674	FE	          mov  a,r6           ; get count
2036	675	A0	          mov  @r0,a          ; store count data for test
2037	676	18	          inc  r0             ; increment pointer
2038			                              ;
2039			                              ; get 12 data bits
2040	677	BF0C	          mov  r7,#0c         ; set up loop counter
2041			                              ;
2042			                              ; get one data
2043	679	BE00	rem5      mov  r6,#00         ; clear count
2044			                              ;
2045	67B	F400	rem6      call delayr         ; 100 us delay between counts
2046			                              ;
2047	67D	FC	          mov  a,r4           ; increment aggregate count
2048	67E	0301	          add  a,#01          ;
2049	680	AC	          mov  r4,a           ;
2050	681	FD	          mov  a,r5           ;
2051	682	1300	          addc a,#00          ;
2052	684	AD	          mov  r5,a           ;
2053			                              ;
2054	685	FD	          mov  a,r5           ; check aggregate count
2055	686	03FD	          add  a,#fd          ; if over 3 * 256 = 768
2056			                              ;   3 generates a carry
2057	688	F6F6	          jc   rem12          ; finish if maximum count reached
2058			                              ;
2059	68A	FE	          mov  a,r6           ; increment count
2060	68B	17	          inc  a              ;
2061	68C	AE	          mov  r6,a           ;
2062			                              ;     
2063	68D	00	          nop                 ; input data
2064	68E	00	          nop                 ;
2065	68F	567B	          jt1  rem6           ; if one, get one data
2066			                              ;
2067	691	FE	          mov  a,r6           ; if zero, check if one low count
2068			                              ;   at least ** 2 ** (#02)
2069	692	03FE	          add  a,#fe          ; 2 (#02) generates a carry
2070	694	E62B	          jnc  rem1           ; if not, restart one sync
2071			                              ;
2072	696	FE	          mov  a,r6           ; if zero, check if one high count
2073			                              ;   no more than ** 7 ** (#07)
2074	697	03F8	          add  a,#f8          ; 8 (#08) generates a carry
2075	699	F62B	          jc   rem1           ; if not, restart one sync
2076			                              ;                    
2077	69B	FE	          mov  a,r6           ; get count
2078	69C	A0	          mov  @r0,a          ; store count data for test
2079	69D	18	          inc  r0             ; increment pointer
2080			                              ;
2081			                              ; get zero data
2082	69E	BE00	          mov  r6,#00         ; clear count
2083			                              ;
2084	6A0	F400	rem7      call delayr         ; 100 us delay between counts
2085			                              ;
2086	6A2	FC	          mov  a,r4           ; increment aggregate count
2087	6A3	0301	          add  a,#01          ;
2088	6A5	AC	          mov  r4,a           ;
2089	6A6	FD	          mov  a,r5           ;
2090	6A7	1300	          addc a,#00          ;
2091	6A9	AD	          mov  r5,a           ;
2092			                              ;
2093	6AA	FD	          mov  a,r5           ; check aggregate count
2094	6AB	03FD	          add  a,#fd          ; if over 3 * 256 = 768
2095			                              ;   3 generates a carry
2096	6AD	F6F6	          jc   rem12          ; finish if maximum count reached
2097			                              ;
2098	6AF	FE	          mov  a,r6           ; increment count
2099	6B0	17	          inc  a              ;
2100	6B1	AE	          mov  r6,a           ;
2101			                              ;     
2102	6B2	00	          nop                 ; input data
2103	6B3	00	          nop                 ;
2104	6B4	46A0	          jnt1 rem7           ; if zero, get zero data
2105			                              ;
2106			                              ; zero bit
2107	6B6	FE	          mov  a,r6           ; if one, check if zero low count
2108			                              ;   at least ** 4 ** (#04)
2109	6B7	03FC	          add  a,#fc          ; 4 (#04) generates a carry
2110	6B9	E62B	          jnc  rem1           ; if not, restart one sync
2111	6BB	FE	          mov  a,r6           ; if one, check if zero high count
2112			                              ;   no more than ** 10 ** (#0a)
2113	6BC	03F5	          add  a,#f5          ; 11 (#0b) generates a carry
2114	6BE	F6C3	          jc   rem8           ; if not, check if one bit
2115			                              ;
2116	6C0	97	          clr  c              ; set zero bit
2117	6C1	C4CF	          jmp  rem9           ;
2118			                              ;
2119			                              ; one bit
2120	6C3	FE	rem8      mov  a,r6           ; if one, check if zero low count
2121			                              ;   at least ** 11 ** (#0b)
2122	6C4	03F5	          add  a,#f5          ; 11 (#0b) generates a carry
2123	6C6	E62B	          jnc  rem1           ; if not, restart one sync
2124	6C8	FE	          mov  a,r6           ; if one, check if zero high count
2125			                              ;   no more than ** 17 ** (#11)
2126	6C9	03EE	          add  a,#ee          ; 18 (#12) generates a carry
2127	6CB	F62B	          jc   rem1           ; if not, restart one sync
2128			                              ;
2129			                              ; 
2130	6CD	97	          clr  c              ; set one bit
2131	6CE	A7	          cpl  c              ;
2132			                              ;
2133	6CF	FB	rem9      mov  a,r3           ; get msb
2134	6D0	67	          rrc  a              ; rotate msb right through carry
2135	6D1	AB	          mov  r3,a           ; store msb
2136	6D2	FA	          mov  a,r2           ; get lsb
2137	6D3	67	          rrc  a              ; rotate lsb right through carry
2138	6D4	AA	          mov  r2,a           ; store lsb
2139			                              ;
2140	6D5	FE	          mov  a,r6           ; get count
2141	6D6	A0	          mov  @r0,a          ; store count data for test
2142	6D7	18	          inc  r0             ; increment pointer
2143			                              ;
2144	6D8	EF79	          djnz r7,#rem5       ; repeat for 12 bits
2145			                              ;                         
2146			                              ; rotate result right 4
2147	6DA	BF04	          mov  r7,#04         ; set up loop counter
2148	6DC	FB	rem10     mov  a,r3           ; get msb
2149	6DD	67	          rrc  a              ; rotate msb right through carry
2150	6DE	AB	          mov  r3,a           ; store msb
2151	6DF	FA	          mov  a,r2           ; get lsb
2152	6E0	67	          rrc  a              ; rotate lsb right through carry
2153	6E1	AA	          mov  r2,a           ; store lsb
2154	6E2	EFDC	          djnz r7,#rem10      ; repeat 4 times
2155			                              ;
2156			                              ; complete cycle
2157	6E4	F400	rem11     call delayr         ; 100 us delay between counts
2158			                              ;
2159	6E6	FC	          mov  a,r4           ; increment aggregate count
2160	6E7	0301	          add  a,#01          ;
2161	6E9	AC	          mov  r4,a           ;
2162	6EA	FD	          mov  a,r5           ;
2163	6EB	1300	          addc a,#00          ;
2164	6ED	AD	          mov  r5,a           ;
2165			                              ;
2166	6EE	FD	          mov  a,r5           ; check aggregate count
2167	6EF	03FD	          add  a,#fd          ; if over 3 * 256 = 768
2168			                              ;   3 generates a carry
2169	6F1	E6E4	          jnc  rem11          ; continue until maximum count reached
2170			                              ;
2171	6F3	2301	          mov  a,#01          ; return with 0x01 in a and remote
2172	6F5	83	          ret                 ;   values in r3 and r2
2173			                              ;
2174	6F6	27	rem12     clr  a              ; if no data, return 0x00 in a
2175	6F7	83	          ret                 ;
2176			;
2177			;
2178			          neworg 700
2178	6F8	00	;
2178	6F9	00	;
2178	6FA	00	;
2178	6FB	00	;
2178	6FC	00	;
2178	6FD	00	;
2178	6FE	00	;
2178	6FF	00	;
2179			;
2180			;
2181			; ----------------------------
2182			;  delayr subroutine
2183			; ----------------------------
2184			;
2185			;  uses a
2186			;
2187			;  11Mhz crystal
2188			;    (1/11M) * 3(prescaler) * 5(machine cycles) = 1.3636us
2189			;
2190			;  100us delay = 73 program cycles
2191			;    average time between inputs is 22 cycles
2192			;    therefore, need 51 cycles = 2 + 5 + 3(a) + 2
2193			;    a = 14 (#0e)
2194			;                             
2195			;  Sample rate test output on P2.7 (pin 38)
2196			;
2197	700	230E	delayr    mov  a,#0e          ; 2 cycles
2198	702	9A7F	          anl  p2,#7f         ; test output adds 5 cycles
2199	704	00	          nop                 ;
2200	705	8A80	          orl  p2,#80         ; 
2201	707	07	delayr1   dec  a              ; 3(a) cycles
2202	708	9607	          jnz  delayr1        ; 
2203	70A	83	          ret                 ; 2 cycles
2204			;
2205			;
2206			; ----------------------------
2207			;  update subroutine
2208			; ----------------------------
2209			;
2210			;  m60 to m6b = standard data 0-b
2211			;  m70 to m7b = alternate data 0-b
2212			;
2213			;  uses a, r0, r4, r5, r6 and r7
2214			;  call to output subroutine uses a, r0, r1, r2 and r3  
2215			;  call to input subroutine uses all registers
2216			;
2217	70B	AC	update    mov  r4,a           ; store phase data
2218	70C	BD6B	          mov  r5,#6b         ; initialize pointer to std data set
2219	70E	BE7B	          mov  r6,#7b         ; initialize pointer to alt data set
2220	710	BF0B	          mov  r7,#0b         ; initialize loop counter    
2221			                              ;
2222	712	FD	update0   mov  a,r5           ; set pointer
2223	713	A8	          mov  r0,a           ;
2224	714	F0	          mov  a,@r0          ; get data
2225	715	5370	          anl  a,#70          ; strip lower bits
2226	717	DC	          xrl  a,r4           ; test if correct phase
2227	718	9622	          jnz  update1        ;
2228	71A	F0	          mov  a,@r0          ; combine data and output address
2229	71B	530F	          anl  a,#0f          ; strip phase and flash bits
2230	71D	47	          swap a              ; add counter as address
2231	71E	4F	          orl  a,r7           ;
2232	71F	47	          swap a              ;
2233	720	F444	          call output         ;
2234			                              ;
2235	722	FD	update1   mov  a,r5           ; set pointer
2236	723	A8	          mov  r0,a           ;
2237	724	F0	          mov  a,@r0          ; get data
2238	725	5380	          anl  a,#80          ; test flash bit
2239	727	C639	          jz   update2        ;
2240			                              ;
2241	729	FE	          mov  a,r6           ; set pointer
2242	72A	A8	          mov  r0,a           ;
2243	72B	F0	          mov  a,@r0          ; get data
2244	72C	5370	          anl  a,#70          ; strip bits
2245	72E	DC	          xrl  a,r4           ; test if correct phase
2246	72F	9639	          jnz  update2        ;
2247	731	F0	          mov  a,@r0          ; combine data and output address
2248	732	530F	          anl  a,#0f          ; strip phase and flash bits
2249	734	47	          swap a              ; add counter as address
2250	735	4F	          orl  a,r7           ;
2251	736	47	          swap a              ;
2252	737	F444	          call output         ;
2253			                              ;
2254	739	CD	update2   dec  r5             ;
2255	73A	CE	          dec  r6             ;
2256	73B	CF	          dec  r7             ;
2257	73C	FF	          mov  a,r7           ;
2258	73D	D3FF	          xrl  a,#ff          ;
2259	73F	9612	          jnz  update0        ;
2260	741	B418	          call input          ;
2261	743	83	          ret                 ;
2262			;
2263			;
2264			; ----------------------------
2265			;  output subroutine
2266			; ----------------------------
2267			;
2268			;  msn in a contains 4 bit port address
2269			;  lsn in a contains 4 bit port data
2270			;
2271			;  m26 = bus output
2272			;  m27 = port 1 output
2273			;
2274			;    uses a, r2 and r3
2275			;    calls jumpa subroutine which uses a, r0 and r1
2276			;
2277	744	AA	output    mov  r2,a           ; store command     
2278	745	47	          swap a              ; determine address
2279	746	530F	          anl  a,#0f          ;
2280	748	AB	          mov  r3,a           ; store address in r3
2281	749	FA	          mov  a,r2           ; get command     
2282	74A	530F	          anl  a,#0f          ; determine data
2283	74C	AA	          mov  r2,a           ; store data in r2
2284			                              ;
2285	74D	FB	          mov  a,r3           ; decode address
2286	74E	E7	          rl   a              ;
2287	74F	F4BB	          call jumpa          ;
2288	751	E469	          jmp  output0        ;       
2289	753	E472	          jmp  output1        ;       
2290	755	E47E	          jmp  output2        ;       
2291	757	E487	          jmp  output3        ;       
2292	759	E493	          jmp  output4        ;       
2293	75B	E498	          jmp  output5        ;       
2294	75D	E49D	          jmp  output6        ;       
2295	75F	E4A2	          jmp  output7        ;       
2296	761	E4A7	          jmp  output8        ;       
2297	763	E4AC	          jmp  output9        ;       
2298	765	E4B1	          jmp  outputa        ;       
2299	767	E4B6	          jmp  outputb        ;       
2300			                              ;
2301	769	B926	output0   mov  r1,#26         ; get bus ouptut
2302	76B	F1	          mov  a,@r1          ;
2303	76C	53F0	          anl  a,#f0          ; update bus output
2304	76E	4A	          orl  a,r2           ;
2305	76F	A1	          mov  @r1,a          ;
2306	770	02	          outl bus,a          ; output data
2307	771	83	          ret                 ;
2308	772	FA	output1   mov  a,r2           ; put data in correct nibble
2309	773	47	          swap a              ;
2310	774	AA	          mov  r2,a           ;
2311	775	B926	          mov  r1,#26         ; get bus ouptut
2312	777	F1	          mov  a,@r1          ;
2313	778	530F	          anl  a,#0f          ; update bus output
2314	77A	4A	          orl  a,r2           ;
2315	77B	A1	          mov  @r1,a          ;
2316	77C	02	          outl bus,a          ; output data
2317	77D	83	          ret                 ;
2318	77E	B927	output2   mov  r1,#27         ; get port ouptut
2319	780	F1	          mov  a,@r1          ;
2320	781	53F0	          anl  a,#f0          ; update port output
2321	783	4A	          orl  a,r2           ;
2322	784	A1	          mov  @r1,a          ;
2323	785	39	          outl p1,a           ; output data
2324	786	83	          ret                 ;
2325	787	FA	output3   mov  a,r2           ; put data in correct nibble
2326	788	47	          swap a              ;
2327	789	AA	          mov  r2,a           ;
2328	78A	B927	          mov  r1,#27         ; get port ouptut
2329	78C	F1	          mov  a,@r1          ;
2330	78D	530F	          anl  a,#0f          ; update port output
2331	78F	4A	          orl  a,r2           ;
2332	790	A1	          mov  @r1,a          ;
2333	791	39	          outl p1,a           ; output data
2334	792	83	          ret                 ;
2335	793	9AEF	output4   anl  p2,#ef         ; select 8243-0
2336	795	FA	          mov  a,r2           ;
2337	796	3C	          movd p4,a           ; output data
2338	797	83	          ret                 ;
2339	798	9AEF	output5   anl  p2,#ef         ; select 8243-0
2340	79A	FA	          mov  a,r2           ;
2341	79B	3D	          movd p5,a           ; output data
2342	79C	83	          ret                 ;
2343	79D	9AEF	output6   anl  p2,#ef         ; select 8243-0
2344	79F	FA	          mov  a,r2           ;
2345	7A0	3E	          movd p6,a           ; output data
2346	7A1	83	          ret                 ;
2347	7A2	9AEF	output7   anl  p2,#ef         ; select 8243-0
2348	7A4	FA	          mov  a,r2           ;
2349	7A5	3F	          movd p7,a           ; output data
2350	7A6	83	          ret                 ;
2351	7A7	8A10	output8   orl  p2,#10         ; select 8243-1
2352	7A9	FA	          mov  a,r2           ;
2353	7AA	3C	          movd p4,a           ; output data
2354	7AB	83	          ret                 ;
2355	7AC	8A10	output9   orl  p2,#10         ; select 8243-1
2356	7AE	FA	          mov  a,r2           ;
2357	7AF	3D	          movd p5,a           ; output data
2358	7B0	83	          ret                 ;
2359	7B1	8A10	outputa   orl  p2,#10         ; select 8243-1
2360	7B3	FA	          mov  a,r2           ;
2361	7B4	3E	          movd p6,a           ; output data
2362	7B5	83	          ret                 ;
2363	7B6	8A10	outputb   orl  p2,#10         ; select 8243-1
2364	7B8	FA	          mov  a,r2           ;
2365	7B9	3F	          movd p7,a           ; output data
2366	7BA	83	          ret                 ;
2367			;
2368			;
2369			; ----------------------------
2370			;  jumpa subroutine
2371			; ----------------------------
2372			;
2373			;    increments pc by value in a
2374			;    uses a and r0 and r1 
2375			;
2376			;    18 cycles, not counting call to subroutine
2377			;
2378			;    stack pointer begins at m8 and goes to m17
2379			;    address lsb is m8, msb is m9 bit 0-3
2380			;    saved psw is m9 bits 4-7 BS, F0, AC and CY 
2381			;
2382			;    this program stores an updated return address in the stack,
2383			;    and then returns
2384			;
2385			;    Note: overflow (i.e. pc + a > 0x7ff would change register bank
2386			;    select to 1 - a real problem
2387			;
2388	7BB	A9	jumpa     mov  r1,a           ; store jump value
2389	7BC	C7	          mov  a,psw          ; get psw
2390	7BD	5307	          anl  a,#07          ; get stack pointer value
2391	7BF	E7	          rl   a              ; double value
2392	7C0	0306	          add  a,#06          ; add base value (8 - 2)
2393	7C2	A8	          mov  r0,a           ; create stack pointer pointer
2394	7C3	F0	          mov  a,@r0          ; get lsb return value
2395	7C4	69	          add  a,r1           ; add jump value to the lsb return value
2396	7C5	A0	          mov  @r0,a          ; store new lsb return value
2397	7C6	18	          inc  r0             ; increment pointer
2398	7C7	F0	          mov  a,@r0          ; get msb return value and psw
2399	7C8	1300	          addc a,#00          ; add carry to msb return value
2400	7CA	A0	          mov  @r0,a          ; store msb return value         
2401	7CB	83	          ret
2402			;
2403			;
2404			; ----------------------------
2405			;  d025s subroutine
2406			; ----------------------------
2407			;
2408			;    approximately 1/4 second
2409			;    uses a, r2 and r3
2410			;
2411	7CC	BAFF	d025s     mov r2,#ff          ; 
2412	7CE	BBFF	          mov r3,#ff          ;
2413	7D0	FA	          mov a,r2            ; store lower count
2414	7D1	AA	d025s1    mov r2,a            ; restore lower count to r2
2415	7D2	EAD2	d025s2    djnz r2,#d025s2     ; loop for standard delay
2416	7D4	EBD1	          djnz r3,#d025s1     ; after first loop completes,
2417	7D6	83	          ret                 ;   will continue
2418			;
2419			;
2420			; ----------------------------
2421			;  driver subroutines (cont)
2422			; ----------------------------
2423			;
2424			;    uses a, r0
2425			;
2426	7D7	B867	cls7r     mov  r0,#67         ;
2427	7D9	B00E	          mov  @r0,#ccls7r    ;
2428	7DB	83	          ret                 ;
2429			                              ;
2430	7DC	B867	cls7y     mov  r0,#67         ;
2431	7DE	B00D	          mov  @r0,#ccls7y    ;
2432	7E0	83	          ret                 ;
2433			                              ;
2434	7E1	B867	cls7yf    mov  r0,#67         ;
2435	7E3	B0BF	          mov  @r0,#ccls7yf   ;
2436	7E5	B877	          mov  r0,#77         ;
2437	7E7	B00D	          mov  @r0,#ccls7y    ;
2438	7E9	83	          ret                 ;
2439			                              ;
2440	7EA	B867	cls7g     mov  r0,#67         ;
2441	7EC	B00B	          mov  @r0,#ccls7g    ;
2442	7EE	83	          ret                 ;
2443			                              ;
2444	7EF	B867	cls7gf    mov  r0,#67         ;
2445	7F1	B0BF	          mov  @r0,#ccls7gf   ;
2446	7F3	B877	          mov  r0,#77         ;
2447	7F5	B00B	          mov  @r0,#ccls7g    ;
2448	7F7	83	          ret                 ;
2449			                              ;
2450	7F8	B869	cls9y     mov  r0,#69         ;
2451	7FA	B02D	          mov  @r0,#ccls9y    ;
2452	7FC	83	          ret                 ;
2453			                              ;
2454			                              ;
2455			;
2456			;
2457			;
 --- Warning line 849 - 11 bit label start2 being used as 8 bit constant
 --- Warning - unused label d4h (4A2)
 --- Warning - unused label d1m (4B6)
 --- Warning - unused label d30s (4C0)
 --- Warning - unused label d5s (4D4)

     mca.exe v2.0 11.22.99
     Assembly complete - 0 errors
                       - 5 warnings
                       - last address written 7fc
*/

