(
p = SerialPort("/dev/max485", 9600, crtscts: true);
)

(
p = DummySerial()
)


(
~dirt.soundLibrary.addSynth(\motors,
	(play: {
		var prms = currentEnvironment,
		serialParams = [prms.device.value,
			prms.dur.value,
			prms.n.value,
			prms.speed.value];
		serialParams.join(" ").postln;

		p.putAll(serialParams.join(" "));
		p.put($\n);
		true
	})
);
)
currentEnvironment.asAssociations.first.value


(
p.putAll("dcarm 0.2 120 0.9\n");
)


