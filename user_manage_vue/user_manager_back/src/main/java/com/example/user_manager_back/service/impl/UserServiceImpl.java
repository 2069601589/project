package com.example.user_manager_back.service.impl;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.user_manager_back.common.ErrorCode;
import com.example.user_manager_back.exception.BusinessException;
import com.example.user_manager_back.model.domain.User;
import com.example.user_manager_back.service.UserService;
import com.example.user_manager_back.mapper.UserMapper;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.util.DigestUtils;

import javax.servlet.http.HttpServletRequest;
import java.nio.charset.StandardCharsets;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.example.user_manager_back.contant.UserContant.USER_LOGIN_STATUS;

/**
* @author 华硕
* @description 针对表【user(用户)】的数据库操作Service实现
* @createDate 2025-02-26 16:49:53
*/
@Service
@Slf4j
public class UserServiceImpl extends ServiceImpl<UserMapper, User>
    implements UserService{
    //盐值
    private static final String SALT = "wj_project";


    @Override
    public long userRegister(String userAccount, String userPassword, String checkPassword) {
        //校验
        //1 都不为空
        if (userAccount == null || userPassword == null || checkPassword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        //2用户名大于4
        if (userAccount.length() < 4) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "用户账号过短");
        }
        //3密码长度大于8
        if (userPassword.length() < 8) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "密码过短");
        }
        //5账户不包含特殊字符
//        Pattern pattern = Pattern.compile("^[a-zA-Z0-9_]+$");
//        String specialChars = "`~!@#$%^&*()+=|{}':;',[].<>/?～！@#￥%……&*（）——+|{}【】'；：""'。，、？";
        //待优化
        String validPattern = "^[a-zA-Z0-9_]+$";
        Matcher matcher = Pattern.compile(validPattern).matcher(userAccount);
        if (!matcher.matches()) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "账户包含特殊字符");
        }
        //6密码和校验密码相同
        if (!userPassword.equals(checkPassword)) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "密码和校验密码不相同");
        }
        //4账户不能重复
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userAccount", userAccount);
        long count = this.count(queryWrapper);
        if (count > 0) {
            throw new BusinessException(ErrorCode.USER_EXIST, "账户已存在");
        }

        //对密码进行加密

        String encryptPassword = DigestUtils.md5DigestAsHex((userAccount + SALT).getBytes(StandardCharsets.UTF_8));
        //向数据库插入用户数据
        User user = new User();
        user.setUserAccount(userAccount);
        user.setUserPassword(encryptPassword);
        boolean result=this.save(user);
        if(!result) return -1;
        return user.getId();
    }

    @Override
    public User userLogin(String userAccount, String userPassword, HttpServletRequest request) {
        //1.校验
        //1.非空
        if (userAccount == null || userPassword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "参数为空");
        }
        //2.账户长度不小于4位
        if (userAccount.length() < 4) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "账户过短");
        }
        //3.密码长度不小于8位
        if (userPassword.length() < 8) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "密码过短");
        }
        //4.账户不包含特殊字符
        String validPattern = "^[a-zA-Z0-9_]+$";
        Matcher matcher = Pattern.compile(validPattern).matcher(userAccount);
        if (!matcher.matches()) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "账户包含特殊字符");
        }
        //2.校验密码是否输入正确
        String encryptPassword = DigestUtils.md5DigestAsHex((userAccount + SALT).getBytes(StandardCharsets.UTF_8));
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("userAccount", userAccount);
        queryWrapper.eq("userPassword", encryptPassword);
        User user = this.getOne(queryWrapper);
        if (user == null) {
            log.info("login failed , userAccount not match userPassword");
            throw new BusinessException(ErrorCode.USER_LOGIN_ERROR, "账户或密码错误");
        }

        //3.用户脱敏
        User safeUser = getSafeUser(user);

        //4.记录用户登录态
        request.getSession().setAttribute(USER_LOGIN_STATUS, safeUser);
        return safeUser;
    }

    @Override
    public User getSafeUser(User originUser) {
        if (originUser == null) {
            return null;
        }
        User safeUser = new User();
        safeUser.setId(originUser.getId());
        safeUser.setUsername(originUser.getUsername());
        safeUser.setUserAccount(originUser.getUserAccount());
        safeUser.setAvatarUrl(originUser.getAvatarUrl());
        safeUser.setGender(originUser.getGender());
        safeUser.setPhone(originUser.getPhone());
        safeUser.setEmail(originUser.getEmail());
        safeUser.setUserStatus(originUser.getUserStatus());
        safeUser.setCreateTime(originUser.getCreateTime());
        safeUser.setUserRole(originUser.getUserRole());
        return safeUser;
    }

    @Override
    public int userLogout(HttpServletRequest request) {
        //移除登录态
        request.getSession().removeAttribute(USER_LOGIN_STATUS);
        return 0;
    }
}




