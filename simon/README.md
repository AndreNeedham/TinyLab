"Simon"-style  repeat game for TinyLab

Andre Needham

Similar to the old Simon repeat game.  The TinyLab will flash one of four LEDs.  Within a couple seconds, you must press the button under the LED that flashed.  Now TinyLab will flash that LED followed by another one.  Press the buttons under the LEDs in sequence.  Keep going until you fail (or you hit 64 in a row at which point the game will probably stop.  I haven't tested that.)

Known issues: the random seed is set to analog input 0 as some Arduino examples show, but it seems like that isn't too random on TinyLab.  I should really look into that.

NOTE: TinyLab's buzzer is on A1.  It doesn't sound very good, so I hooked an 8 ohm, .2 watt speaker between pin 5 and ground.
It's loud enough, sounds good, and only seems to use 9ma or so, way under the maximum pin output current.
Change SPKR_PIN from 5 to A1 if you really want to hear the buzzer.
