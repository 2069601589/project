package com.example.user_manager_back.service;

import java.util.Date;

import com.example.user_manager_back.model.domain.User;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import javax.annotation.Resource;

@SpringBootTest
class UserServiceTest {
    @Resource
    private UserService userService;

    @Disabled("测试通过")
    @Test
    void testAddUser() {
        User user = new User();
        user.setId(0L);
        user.setUsername("小明");
        user.setUserAccount("2025");
        user.setAvatarUrl("aikun");
        user.setGender(0);
        user.setUserPassword("123");
        user.setPhone("123");
        user.setEmail("456");
        user.setUserStatus(0);
        user.setCreateTime(new Date());
        user.setUpdateTime(new Date());
        user.setIsDelete(0);
        boolean result = userService.save(user);
        System.out.println(user.getId());
        Assertions.assertTrue(result);
    }

    @Test
    void userRegister() {
        // 1. 测试参数为空
        long result1 = userService.userRegister(null, null, null);
        Assertions.assertEquals(-1, result1);

        // 2. 测试账号小于4位
        long result2 = userService.userRegister("123", "12345678", "12345678");
        Assertions.assertEquals(-1, result2);

        // 3. 测试密码小于8位
        long result3 = userService.userRegister("123456", "123", "123");
        Assertions.assertEquals(-1, result3);

        // 4. 测试账号重复
        long result4 = userService.userRegister("2025", "87654321", "87654321");
        Assertions.assertEquals(-1, result4);

        // 5. 测试账号包含特殊字符
        long result5 = userService.userRegister("小@123", "12345678", "12345678");
        Assertions.assertEquals(-1, result5);

        // 6. 测试密码和校验码不一致
        long result6 = userService.userRegister("1234567", "12345678", "87654321");
        Assertions.assertEquals(-1, result6);

        // 测试正常情况
        long successResult = userService.userRegister("123654", "12345678", "12345678");
        Assertions.assertTrue(successResult > 0);
    }
}