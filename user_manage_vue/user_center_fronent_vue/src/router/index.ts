import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";
import UserLoginPage from "@/pages/user/UserLoginPage.vue";
import UserRegisterPage from "@/pages/user/UserRegisterPage.vue";
import UserManagePage from "@/pages/admin/UserManagePage.vue";
import HomePage from "@/pages/HomePage.vue";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/",
    name: "主页",
    component: HomePage,
  },
  {
    path: "/user/login",
    name: "用户登录",
    component: UserLoginPage,
  },
  {
    path: "/user/register",
    name: "用户注册",
    component: UserRegisterPage,
  },
  {
    path: "/admin/userManage",
    name: "用户管理",
    component: UserManagePage,
  },
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
});

export default router;
