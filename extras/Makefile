
SUBDIRS =  copy_to_user interrupts\
           interrupts_2 interrupts_3 sched_interrupts\
	   spinlock spinlock_example tasklets workqueues

all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
