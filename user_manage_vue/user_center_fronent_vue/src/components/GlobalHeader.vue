<template>
  <div id="global-header">
    <a-row :warp="false">
      <a-col flex="200px">
        <div class="title_bar">
          <div class="title">用户管理中心</div>
        </div>
      </a-col>
      <a-col flex="auto">
        <a-menu
          v-model:selectedKeys="current"
          mode="horizontal"
          :items="items"
          @click="doMenuClick"
        />
      </a-col>
      <a-col flex="100px">
        <div v-if="loginUserStore.loginUser.id">
          {{ loginUserStore.loginUser.username ?? "无名" }}
        </div>
        <div v-else>
          <a-button type="primary" href="/user/login">登陆</a-button>
        </div>
      </a-col>
    </a-row>
  </div>
</template>
<script lang="ts" setup>
import { h, ref } from "vue";
import { HomeOutlined, CrownOutlined } from "@ant-design/icons-vue";
import { MenuProps } from "ant-design-vue";
import { useRouter } from "vue-router";
import { useLoginUserStore } from "@/store/useLoginUserStore";

const loginUserStore = useLoginUserStore();
const router = useRouter();
const doMenuClick = ({ key }: { key: string }) => {
  router.push({ path: key });
};

const current = ref<string[]>(["mail"]);
// 监听路由变化
router.afterEach((to) => {
  current.value = [to.path];
});

const items = ref<MenuProps["items"]>([
  {
    key: "/",
    icon: () => h(HomeOutlined),
    label: "主页",
    title: "主页",
  },
  {
    key: "/user/login",
    label: "用户登陆",
    title: "用户登陆",
  },
  {
    key: "/user/register",

    label: "用户注册",
    title: "用户注册",
  },
  {
    key: "/admin/userManage",
    icon: () => h(CrownOutlined),
    label: "用户管理",
    title: "用户管理",
  },
]);
</script>

<style scoped>
.title_bar {
  display: flex;
  align-items: center;
}

.logo {
  width: 20px;
}

.title {
  color: black;
  font-size: 18px;
  font-weight: bold;
}
</style>
