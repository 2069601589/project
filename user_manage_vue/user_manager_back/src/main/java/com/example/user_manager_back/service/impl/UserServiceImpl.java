package com.example.user_manager_back.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.user_manager_back.model.domain.User;
import com.example.user_manager_back.service.UserService;
import com.example.user_manager_back.mapper.UserMapper;
import org.springframework.stereotype.Service;

/**
* @author 华硕
* @description 针对表【user(用户)】的数据库操作Service实现
* @createDate 2025-02-26 16:49:53
*/
@Service
public class UserServiceImpl extends ServiceImpl<UserMapper, User>
    implements UserService{

}




