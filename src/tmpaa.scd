(
p = SerialPort("/dev/max485", 9600, crtscts: true);
)

(
p = DummySerial()
)


(
~dirt.soundLibrary.addSynth(\motors,
	(type: \midi, play: {
		var prms = currentEnvironment,
		serialParams = [prms.device.value,
			prms.dur.value,
			prms.n.value,
			prms.speed.value];
		p.putAll(serialParams.join(" "));
		p.put($\n);
		serialParams.join(" ").postln;
		true
	})
);
)
currentEnvironment.asAssociations.first.value


(
p.putAll("dcarm 1 90 0.9\n");
)