LEX = flex
LEXFLAGS = -i
YACC = bison
YACCFLAGS = -d

# intermediate things to be deleted if "make clean"
INTERMEDIATES += \
${addprefix $(ROOT)/parser/, \
grammar.cpp \
grammar.h \
lexer.cpp \
}

$(ROOT)/parser/grammar.cpp $(ROOT)/parser/grammar.h: $(ROOT)/parser/grammar.y
	@echo 'Building: $@'
	$(YACC) $(YACCFLAGS) -o$@ $<
	mv  $(ROOT)/parser/grammar.hpp $(ROOT)/parser/grammar.h
	@echo 'Finished building: $<'
	@echo ' '

$(ROOT)/parser/lexer.cpp : $(ROOT)/parser/lexer.l
	@echo 'Building: $@'
	$(LEX) $(LEXFLAGS) -o$@ $<
	@echo 'Finished building: $<'
	@echo ' '

CPP_SRCS += \
${addprefix $(ROOT)/parser/, \
BExpression.cpp \
ColOrder.cpp \
EHolder.cpp \
Expression.cpp \
grammar.cpp \
lexer.cpp \
ListCols.cpp \
ListOrdering.cpp \
ListProjections.cpp \
ListSelections.cpp \
Parser.cpp \
PIdent.cpp \
PObject.cpp \
Projection.cpp \
Query.cpp \
TVariable.cpp \
ListColumnValues.cpp \
ListExps.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
parser/%.o: $(ROOT)/parser/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

# @g++ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
#g++ -MM -MG -P -w -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
