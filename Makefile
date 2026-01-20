SHELL := /bin/sh


# Disable old-fashioned suffix rules.
.SUFFIXES :


objs_dir := ./obj
srcs_dir := ./src
hdrs_dir := ./include
docs_dir := ./docs


CC       := cc
CXX      := c++
CXXFLAGS := -fopenmp -Wall -Wextra -g3 -O3
CFLAGS   := -fopenmp -Wall -Wextra -g3 -O3
CPPFLAGS := -I$(hdrs_dir) 


# &> is a Bash feature, use POSIX complient instead.
$(shell mkdir -p $(objs_dir) >/dev/null 2>&1)
$(intcmp $(.SHELLSTATUS),0,,,$(error Directory $(objs_dir) has not been created))


vpath %.o $(objs_dir)
vpath %.c $(srcs_dir)
vpath %.cc $(srcs_dir)
vpath %.h $(hdrs_dir)


$(objs_dir)/%.o : %.c
	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $<


$(objs_dir)/%.o : %.cc
	$(CXX) -c -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


srcs := c_sums.c conduct_experiment.cc conf_thrd_pool.c cxx_sums.cc barrier.cc \
		semaphore.c
		
objs := $(addprefix $(objs_dir)/,$(patsubst %.c,%.o,$(patsubst %.cc,%.c,$(srcs))))
executable := conduct_experiment


.DEFAULT_TARGET := $(executable)
$(executable) : $(objs)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^


conduct_experiment.cc cxx_sums.cc : cxx_sums.h
c_sums.cc : c_sums.h
conf_thrd_pool.cc conduct_experiment.cc : conf_thrd_pool.h
cxx_sums.cc c_sums.cc cxx_sums.h : conf_thrd_pool.h
barrier.cc : barrier.h
sum_reduce.cc : barrier.h conf_thrd_pool.h
semaphore.cc : semaphore.h


.PHONY : echo_vars
echo_vars :
	@echo srcs = $(srcs)
	@echo objs = $(objs)


.PHONY : html
html :
	doxygen Doxyfile


.PHONY : clean
clean :
	$(RM) $(objs_dir)/* $(executable)
	rmdir $(objs_dir)
	$(RM) -rf $(docs_dir)/*

