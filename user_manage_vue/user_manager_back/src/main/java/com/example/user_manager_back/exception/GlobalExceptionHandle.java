package com.example.user_manager_back.exception;
import com.example.user_manager_back.common.BaseResponse;
import com.example.user_manager_back.common.ErrorCode;
import com.example.user_manager_back.common.ResultUtils;
import org.springframework.web.bind.annotation.RestControllerAdvice;

@RestControllerAdvice
public class GlobalExceptionHandle {
   public BaseResponse businessExceptionHandler(BusinessException e) {
      return ResultUtils.error(e.getCode(), e.getMessage(), e.getDescription());
   }
   public BaseResponse runtimeExceptionHandler(RuntimeException e) {
      return ResultUtils.error(ErrorCode.SYSTEM_ERROR, e.getMessage(), "");
   }
}
