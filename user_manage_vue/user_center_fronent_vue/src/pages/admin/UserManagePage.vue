<template>
  <div id="userManagePage">
    <a-input-search
      v-model:value="searchValue"
      placeholder="input search text"
      enter-button
      style="width: 500px; margin-bottom: 20px"
      @search="onSearch"
    />
    <a-table :columns="columns" :data-source="data">
      <template #bodyCell="{ column, record }">
        <template v-if="column.dataIndex === 'avatarUrl'">
          <a-image :src="record.avatarUrl" :width="60" />
        </template>
        <template v-else-if="column.dataIndex === 'userRole'">
          <div v-if="record.userRole === 0">
            <a-tag color="blue">普通用户</a-tag>
          </div>
          <div v-else-if="record.userRole === 1">
            <a-tag color="green">管理员</a-tag>
          </div>
        </template>
        <template v-if="column.dataIndex === 'createTime'">
          <span>{{ new Date(record.createTime).toLocaleString() }}</span>
        </template>
        <template v-else-if="column.key === 'action'">
          <a-button danger @click="doDelete(record.id)">删除</a-button>
        </template>
      </template>
    </a-table>
  </div>
</template>

<script lang="ts" setup>
import { deleteUser, searchUsers } from "@/api/user";
import { ref } from "vue";
import { message } from "ant-design-vue";

const searchValue = ref("");

const onSearch = async () => {
  await fetchData(searchValue.value);
};

const doDelete = async (id: string) => {
  const ref = await deleteUser(id);
  await fetchData(searchValue.value);
  if (ref.data.code === 0) {
    message.success("删除成功");
  } else {
    message.error("删除失败");
  }
};

const columns = [
  {
    title: "id",
    dataIndex: "id",
  },
  {
    title: "用户名",
    dataIndex: "username",
  },
  {
    title: "账号",
    dataIndex: "userAccount",
  },
  {
    title: "头像",
    dataIndex: "avatarUrl",
  },
  {
    title: "创建时间",
    dataIndex: "createTime",
  },
  {
    title: "用户身份",
    dataIndex: "userRole",
  },
  {
    title: "Action",
    key: "action",
  },
];

const data = ref([]);

const fetchData = async (username = "") => {
  const ref = await searchUsers(username);
  if (ref.data.code === 0) {
    data.value = ref.data.data;
  } else {
    message.error("获取用户列表失败");
  }
};
fetchData();
</script>
