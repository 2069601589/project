package com.example.user_manager_back.common;

import lombok.Getter;

@Getter
public enum ErrorCode {
    /**
     * 成功
     */
    SUCCESS(0, "成功", ""),
    /**
     * 请求参数错误
     */
    PARAMS_ERROR(40001, "请求参数错误", ""),
    USER_NOT_EXIST(40002, "用户不存在", ""),
    NULL_ERROR(40003,"请求参数为空",""),
    USER_EXIST(40004,"用户已存在",""),
    USER_NOT_LOGIN(40005,"用户未登录",""),
    USER_LOGIN_ERROR(40006,"用户名或密码错误",""),
    NO_PERMISSION(40100,"无权限",""),
    SYSTEM_ERROR(50000, "系统错误", "");

    private final int  code;
    private final String message;
    private final String description;

    ErrorCode(int code, String message, String description) {
        this.code = code;
        this.message = message;
        this.description = description;
    }

}
