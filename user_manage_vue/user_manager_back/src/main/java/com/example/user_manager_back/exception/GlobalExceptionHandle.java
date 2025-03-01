package com.example.user_manager_back.exception;
import com.example.user_manager_back.common.BaseResponse;
import com.example.user_manager_back.common.ErrorCode;
import com.example.user_manager_back.common.ResultUtils;
import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;

@RestControllerAdvice
@Slf4j
public class GlobalExceptionHandle {
   @ExceptionHandler(BusinessException.class)
   public BaseResponse businessExceptionHandler(BusinessException e) {
       log.error("业务异常: {}", e.getMessage(), e);
      return ResultUtils.error(e.getCode(), e.getMessage(), e.getDescription());
   }
   @ExceptionHandler(RuntimeException.class)
   public BaseResponse runtimeExceptionHandler(RuntimeException e) {
      log.error("运行时异常", e);
      return ResultUtils.error(ErrorCode.SYSTEM_ERROR, e.getMessage(), "");
   }
}
