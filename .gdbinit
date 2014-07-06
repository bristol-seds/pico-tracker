define blackmagic
	target extended-remote /dev/ttyACM$arg0
end
source gdbscript
source gdbscript-custom
