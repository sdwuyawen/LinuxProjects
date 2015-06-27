objs := a.o b.o

test: $(objs)
	gcc -o $@ $^

# .a.o.d .b.o.d
dep_files := $(foreach f,$(objs),.$(f).d)
dep_files := $(wildcard $(dep_files))

ifneq ($(dep_files),)
  include $(dep_files)
endif

%.o: %.c
	gcc -o $@ -c $< -Wp,-MD,.$@.d

clean:
	rm *.o test

