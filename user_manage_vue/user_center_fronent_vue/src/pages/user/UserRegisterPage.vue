<template>
  <div id="userLoginPage">
    <h2 class="title">用户注册</h2>
    <a-form
      style="max-width: 480px; margin: 0 auto"
      :model="form"
      name="basic"
      label-align="left"
      :label-col="{ span: 4 }"
      :wrapper-col="{ span: 20 }"
      autocomplete="off"
      @finish="handleSubmit"
    >
      <a-form-item
        label="账号"
        name="userAccount"
        :rules="[{ required: true, message: '请输入账号' }]"
      >
        <a-input v-model:value="form.userAccount" placeholder="请输入账号" />
      </a-form-item>

      <a-form-item
        label="密码"
        name="userPassword"
        :rules="[
          { required: true, message: '请输入密码' },
          { min: 8, message: '密码长度至少8位' },
        ]"
      >
        <a-input-password
          v-model:value="form.userPassword"
          placeholder="请输入密码"
        />
      </a-form-item>
      <a-form-item
        label="确认密码"
        name="checkPassword"
        :rules="[
          { required: true, message: '请再次输入密码' },
          { min: 8, message: '密码长度至少8位' },
        ]"
      >
        <a-input-password
          v-model:value="form.checkPassword"
          placeholder="请再次输入密码"
        />
      </a-form-item>
      <a-form-item :wrapper-col="{ offset: 4, span: 20 }">
        <a-button type="primary" html-type="submit">注册</a-button>
      </a-form-item>
    </a-form>
    >
    <div style="height: 220px"></div>
  </div>
</template>

<script lang="ts" setup>
import { reactive } from "vue";
import { useRouter } from "vue-router";
import { message } from "ant-design-vue";
import { userRegister } from "@/api/user";

const form = reactive({
  userAccount: "",
  userPassword: "",
  checkPassword: "",
});

const router = useRouter();

/**
 * 提交表单
 * @param data
 */
const handleSubmit = async () => {
  const res = await userRegister(form);
  // 登录成功，跳转到主页
  if (res.data.code === 0 && res.data.data) {
    message.success("注册成功");
    await router.push({
      path: "/user/login",
      replace: true,
    });
  } else {
    message.error(res.data.message || "注册失败");
  }
};
</script>

<style scoped>
.title {
  text-align: center;
  margin-bottom: 16px;
}
</style>
