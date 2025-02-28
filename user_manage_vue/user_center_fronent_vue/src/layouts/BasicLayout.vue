<template>
  <div id="basic-layout">
    <a-layout>
      <a-layout-header class="header">
        <GlobalHeader />
      </a-layout-header>
      <a-layout-content class="content">
        <router-view />
      </a-layout-content>
      <a-layout-footer class="footer">
        <a href="https://github.com/">github</a>
      </a-layout-footer>
    </a-layout>
  </div>
</template>

<script setup lang="ts">
import GlobalHeader from "@/components/GlobalHeader.vue";
</script>

<style scoped>
#basic-layout .footer {
  background: #efefef;
  text-align: center;
  position: fixed; /*固定在底部*/
  bottom: 0;
  left: 0;
  right: 0;
}

#basic-layout .content {
  padding: 24px; /*防止底部内容被footer覆盖*/
  margin-bottom: 24px;
  background: linear-gradient(#efefef, #fff); /*渐变色*/
}

#basic-layout .header {
  background: #fff;
  color: unset;
  padding-inline: 20px;
}
</style>
