CC:=aarch64-rpi4-linux-uclibc-gcc
CXX:=aarch64-rpi4-linux-uclibc-g++
LD:=aarch64-rpi4-linux-uclibc-g++
RM:= rm

BINDIR:=bin
SRCS:= \
    src/main.c \
	

INCDIR:=inc
OBJDIR:=.o
DEPDIR:=.d



CFLAGS:= -std=c11
CXXFLAGS:= -std=c++17
CPPFLAGS := -g -Wall -Wextra -pedantic -I $(INCDIR) -I /home/lars/info/wifi2linux/buildroot/output/host/include
LDFLAGS:=
LDLIBS:=
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td



OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)


all: $(BINDIR)/btnPower

$(BINDIR)/btnPower: $(OBJS)
	$(LD) $(LDFLAGS) $(LDLIBS) -o $@  $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
	mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d




$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
	mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

.PHONY: clean
clean: 
	$(RM) -r $(OBJDIR) $(DEPDIR)


-include $(DEPS)
