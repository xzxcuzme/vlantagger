.PHONY := clean
bin := vlan
sources := vlan.c
object := vlan.o
objects = filter.o
statLib := libfilter.a

$(bin): $(object) $(statLib)
		gcc $(sources) -o $(bin) -L. -lfilter -g

$(statLib): $(objects)
		ar cr $(statLib) $(objects)

%.o: %.c
		gcc $< -c -g

clean:
	rm -rf $(bin) $(object) $(objects) $(statLib)