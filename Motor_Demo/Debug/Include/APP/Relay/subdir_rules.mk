################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Include/APP/Relay/%.obj: ../Include/APP/Relay/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/Motor_Demo/Motor_Demo" --include_path="C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/include" --include_path="E:/Motor_Demo/Motor_Demo/Include/DSP2833x_headers" --include_path="E:/Motor_Demo/Motor_Demo/Include/DSP2833x_common" --include_path="E:/Motor_Demo/Motor_Demo/Include/APP/Relay" --include_path="E:/Motor_Demo/Motor_Demo/Include/APP/Key" --include_path="E:/Motor_Demo/Motor_Demo/Include/APP/Led" --include_path="E:/Motor_Demo/Motor_Demo/Include/APP" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Include/APP/Relay/$(basename $(<F)).d_raw" --obj_directory="Include/APP/Relay" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


