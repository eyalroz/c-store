CPP_SRCS += \
${addprefix $(ROOT)/OptPlan/, \
Opt_Object.cpp \
Opt_Column.cpp \
Opt_String.cpp \
Opt_Number.cpp \
Opt_Number_Float.cpp \
Opt_Number_Integer.cpp \
Opt_Comparison.cpp \
Opt_Comparison_And.cpp \
Opt_Comparison_Or.cpp \
Opt_Comparison_Ne.cpp \
Opt_Comparison_Lt.cpp \
Opt_Comparison_Le.cpp \
Opt_Comparison_Gt.cpp \
Opt_Comparison_Ge.cpp \
Opt_Comparison_Eq.cpp \
Opt_Join.cpp \
Opt_Agg.cpp \
Opt_Agg_Count.cpp \
Opt_Agg_Avg.cpp \
Opt_Agg_Min.cpp \
Opt_Agg_Max.cpp \
Opt_Agg_Sum.cpp \
Opt_Table.cpp \
Opt_QueryGraph.cpp \
Opt_SnowFlakeJoin.cpp \
Opt_SnowflakeInterestingOrders.cpp \
Opt_SnowFlakes.cpp \
Opt_Plan.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
OptPlan/%.o: $(ROOT)/OptPlan/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '
