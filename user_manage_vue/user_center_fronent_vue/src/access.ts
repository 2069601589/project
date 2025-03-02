import router from "@/router";
import { useLoginUserStore } from "@/store/useLoginUserStore";
import { message } from "ant-design-vue";

router.beforeEach((to, from, next) => {
  const loginUserStore = useLoginUserStore();
  const loginUser = loginUserStore.loginUser;
  const toUrl = to.path;
  if (toUrl.startsWith("/admin")) {
    if (!loginUser || loginUser.userRole !== "1") {
      message.error("该页面仅限管理员访问，请先登陆");
      next(`/user/login?redirect=${toUrl}`);
      return;
    }
  }
  next();
});
