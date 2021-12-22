<script setup>
import { ref } from "vue";

const props = defineProps({
  // avatorUrl: String,
  url: String,
  cover_url: String,
  // upnick: String,
  title: String,
});
</script>

<template>
  <el-row type="flex" justify="center">
    <el-card class="vc-card" :body-style="{ padding: '0px' }">
      <a :href="url">
        <img :src="cover_url" class="image" />
      </a>
      <div class="card-video-title">
        <span>{{title}}</span>
      </div>
    </el-card>
  </el-row>
</template>

<style scoped>
.bottom {
  margin-top: 13px;
  line-height: 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.vc-card {
  margin: 1rem;
}
.image {
  width: 20rem;
  display: block;
}
.card-video-title {
  max-width: 19.6rem;
  min-height: 3rem;
  display: flex;
  align-items: center;
  justify-content: center;
}
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.card-header-uploader {
  display: flex;
  align-items: center;
}
.card-header-uploader-avator {
  margin-right: 0.4rem;
}
</style>
