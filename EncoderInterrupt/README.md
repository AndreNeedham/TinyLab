Encoder test using a timer interrupt to poll 500 times/second

Andre Needham

The TinyLab's rotary encoder can be rotated very quickly, and either of the two signals from it can end up being enabled for a very short time.  Since the only way to read the encoder is via polling (only one of the two pins the encoder is using allows for pin change interrupts), it's very possible to miss encoder changes.  The solution is to set up a timer that calls our encoder check function every .002 second (500 times per second).  In the loop code, we can just check to see if our variable changed, and display the current encoder value.

To compile the code you'll need to get the TimerOne library from https://github.com/PaulStoffregen/TimerOne .

I tried using this timer code in TinyLabs' tinylab_test_code project, and it worked there, and didn't seem to interfere with any other tests (though I didn't test with bluetooth/wifi/zigbee/NRF).