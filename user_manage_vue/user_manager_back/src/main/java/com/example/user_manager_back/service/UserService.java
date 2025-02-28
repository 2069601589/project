package com.example.user_manager_back.service;

import com.example.user_manager_back.model.domain.User;
import com.baomidou.mybatisplus.extension.service.IService;

import javax.servlet.http.HttpServletRequest;

public interface UserService extends IService<User> {
    /**
     * 用户注册
     * @param userAccount 用户账号
     * @param userPassword 用户密码
     * @param checkPassword 校验密码
     * @return 注册结果
     */
    long userRegister(String userAccount, String userPassword, String checkPassword);
    User userLogin(String userAccount, String userPassword, HttpServletRequest request);

    User getSafeUser(User originUser);

    int userLogout(HttpServletRequest request);

}
