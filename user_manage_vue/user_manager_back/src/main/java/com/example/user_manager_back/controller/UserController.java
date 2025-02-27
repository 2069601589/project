package com.example.user_manager_back.controller;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.example.user_manager_back.model.domain.User;
import com.example.user_manager_back.model.domain.request.UserLoginRequest;
import com.example.user_manager_back.model.domain.request.UserRegisterRequest;
import com.example.user_manager_back.service.UserService;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.ArrayList;
import java.util.List;

import static com.example.user_manager_back.contant.UserContant.Admin_User;
import static com.example.user_manager_back.contant.UserContant.USER_LOGIN_STATUS;

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

    //根据用户名查询
    @GetMapping("/search")
    public List<User> searchUsers(String username,HttpServletRequest request) {
        if(!isAdmin(request))
            return new ArrayList<>();
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        if(username != null) {
            queryWrapper.like("username", username);
        }
        return userService.list(queryWrapper);
    }

    //根据id删除
    @PostMapping("/delete")
    public Boolean deleteUsers(@RequestBody int id,HttpServletRequest request) {
        if(!isAdmin(request))
            return false;
        if(id<=0) {
            return false;
        }
        return userService.removeById(id);
    }

    /**
     * 判断是否是管理员
     */
    private Boolean isAdmin(HttpServletRequest request) {
        Object useObj=request.getSession().getAttribute(USER_LOGIN_STATUS);
        User user=(User)useObj;
        return user != null && user.getUserRole() == Admin_User;
    }

}
