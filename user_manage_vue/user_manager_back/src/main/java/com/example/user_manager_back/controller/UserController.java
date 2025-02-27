package com.example.user_manager_back.controller;

import com.example.user_manager_back.model.domain.User;
import com.example.user_manager_back.model.domain.request.UserLoginRequest;
import com.example.user_manager_back.model.domain.request.UserRegisterRequest;
import com.example.user_manager_back.service.UserService;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;

@RestController
@RequestMapping("/user")
public class UserController {
    @Resource
    private UserService userService;

    @PostMapping("/register")
    public long register(@RequestBody UserRegisterRequest userRegisterRequest) {
        if(userRegisterRequest == null) {
            return -1;
        }
        String userAccount = userRegisterRequest.getUserAccount();
        String userPassword = userRegisterRequest.getUserPassword();
        String checkPassword = userRegisterRequest.getCheckCode();
        //判断参数非空
        if (userAccount == null || userPassword == null || checkPassword == null) {
            return -1;
        }
        return userService.userRegister(userAccount, userPassword, checkPassword);
    }

    @PostMapping("/login")
    public User login(@RequestBody UserLoginRequest userLoginRequest, HttpServletRequest request) {
        if(userLoginRequest == null) {
            return null;
        }
        String userAccount = userLoginRequest.getUserAccount();
        String userPassword = userLoginRequest.getUserPassword();
        //判断参数非空
        if (userAccount == null || userPassword == null) {
            return null;
        }
        return userService.userLogin(userAccount, userPassword,request);
    }
}
