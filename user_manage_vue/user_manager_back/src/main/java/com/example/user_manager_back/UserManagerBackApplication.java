package com.example.user_manager_back;

import org.mybatis.spring.annotation.MapperScan;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
@MapperScan("com.example.user_manager_back.mapper")
public class UserManagerBackApplication {

    public static void main(String[] args) {
        SpringApplication.run(UserManagerBackApplication.class, args);
    }

}
