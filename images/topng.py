import sys
import os
name = sys.argv[1]
width = float(sys.argv[2])
fil = open(name + ".svg.hb", "r")
out = open(name + ".png.hb", "w")
origdim = fil.readline().split(' ')
ratio = width/float(origdim[0])
print(str(float(origdim[0])*ratio) + " " + str(float(origdim[1])*ratio), file=out)

for line in fil:
	sp = line.strip().split(' ')
	print(sp[0] + " ", end="", file=out)
	print(" ".join(map(lambda x: str(ratio*float(x)), sp[1:])), file=out)

fil.close()
out.close()

os.system("inkscape -z -e %s.png -w %f %s.svg" % (name, width, name))
