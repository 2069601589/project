package com.example.user_manager_back.controller;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.example.user_manager_back.common.BaseResponse;
import com.example.user_manager_back.common.ErrorCode;
import com.example.user_manager_back.common.ResultUtils;
import com.example.user_manager_back.exception.BusinessException;
import com.example.user_manager_back.model.domain.User;
import com.example.user_manager_back.model.domain.request.UserLoginRequest;
import com.example.user_manager_back.model.domain.request.UserRegisterRequest;
import com.example.user_manager_back.service.UserService;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.List;

import static com.example.user_manager_back.contant.UserContant.Admin_User;
import static com.example.user_manager_back.contant.UserContant.USER_LOGIN_STATUS;

@RestController
@RequestMapping("/user")
@CrossOrigin(origins = {"http://localhost:3000"}, allowCredentials = "true")
public class UserController {
    @Resource
    private UserService userService;

    @PostMapping("/register")
    public BaseResponse<Long> register(@RequestBody UserRegisterRequest userRegisterRequest) {
        if(userRegisterRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        String userAccount = userRegisterRequest.getUserAccount();
        String userPassword = userRegisterRequest.getUserPassword();
        String checkPassword = userRegisterRequest.getCheckPassword();
        //判断参数非空
        if (userAccount == null || userPassword == null || checkPassword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        long result=userService.userRegister(userAccount, userPassword, checkPassword);
        return ResultUtils.success(result);
    }

    @PostMapping("/login")
    public BaseResponse<User> login(@RequestBody UserLoginRequest userLoginRequest, HttpServletRequest request) {
        if(userLoginRequest == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        String userAccount = userLoginRequest.getUserAccount();
        String userPassword = userLoginRequest.getUserPassword();
        //判断参数非空
        if (userAccount == null || userPassword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        User user= userService.userLogin(userAccount, userPassword,request);
        return ResultUtils.success(user);
    }

    @GetMapping("/current")
    public BaseResponse<User> getCurrentUser(HttpServletRequest request) {
        Object userObj = request.getSession().getAttribute(USER_LOGIN_STATUS);
        User currentUser = (User) userObj;
        if (currentUser == null) {
            throw new BusinessException(ErrorCode.USER_LOGIN_ERROR);
        }
        long userId = currentUser.getId();
        // TODO 校验用户是否合法
        User user = userService.getById(userId);
        User safeUser = userService.getSafeUser(user);
        return ResultUtils.success(safeUser);
    }

    //根据用户名查询
    @GetMapping("/search")
    public BaseResponse<List<User>> searchUsers(String username,HttpServletRequest request) {
        if(!isAdmin(request))
            throw new BusinessException(ErrorCode.NO_PERMISSION);
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        if(username != null) {
            queryWrapper.like("username", username);
        }
        List<User> user= userService.list(queryWrapper);
        return ResultUtils.success(user);
    }

    //根据id删除
    @PostMapping("/delete")
    public BaseResponse<Boolean> deleteUsers(@RequestBody int id,HttpServletRequest request) {
        if(!isAdmin(request))
            throw new BusinessException(ErrorCode.NO_PERMISSION);
        if(id<=0) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        Boolean result=userService.removeById(id);
        return ResultUtils.success(result);
    }

    /**
     * 判断是否是管理员
     */
    private Boolean isAdmin(HttpServletRequest request) {
        Object useObj=request.getSession().getAttribute(USER_LOGIN_STATUS);
        User user=(User)useObj;
        return user != null && user.getUserRole() == Admin_User;
    }

    @PostMapping("/logout")
    public BaseResponse<Integer> logout(HttpServletRequest request) {
        if(request == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }
        int result= userService.userLogout(request);
        return ResultUtils.success(result);
    }
}
