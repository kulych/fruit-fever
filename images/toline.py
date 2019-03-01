import sys
name = sys.argv[1]
fil = open(name + ".svg.sq", "r")
out = open(name + ".svg.hb", "w")
fline = fil.readline().strip()
height = float(fline.split(' ')[1])
print(fline, file=out)

for line in fil:
	tmp = line.strip().split(' ')
	if (tmp[0] == "C"):
		x = float(tmp[1])
		y = float(tmp[2])
		d = float(tmp[3])
		#inkscape has math-style axis, y=0 is bottom
		#speaks about left bottom corner of the circle
		#i want centerx centery radius
		print("C %.2f %.2f %.2f" % (x+d/2, height-y-d/2, d/2), file=out)
	elif (tmp[0] == "L"):
		x = float(tmp[1])
		y = float(tmp[2])
		x2 = float(tmp[3])
		y2 = float(tmp[4])
		print("L %.2f %.2f %.2f %.2f" % (x, height-y, x2, height-y2), file=out)

fil.close()
out.close()

