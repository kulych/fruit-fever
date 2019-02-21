import sys
name = sys.argv[1]
fil = open(name + ".svg.sq", "r")
out = open(name + ".svg.hb", "w")
fline = fil.readline().strip()
height = float(fline.split(' ')[1])
print(fline, file=out)

for line in fil:
	tmp = line.strip().split(' ')
	x = float(tmp[0])
	y = float(tmp[1])
	d = float(tmp[2])
	#inkscape has math-style axis, y=0 is bottom
	#speaks about left bottom corner of the circle
	#i want centerx centery radius
	print("%.2f %.2f %.2f" % (x+d/2, height-y-d/2, d/2), file=out)

fil.close()
out.close()

