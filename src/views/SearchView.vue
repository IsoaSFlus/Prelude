<script setup>
import { Search } from "@element-plus/icons-vue";
import AlbumCard from "../components/AlbumCard.vue";
import { ref, onMounted } from "vue";
const input = ref("");
const albums = ref(Array());
let search = () => {
  console.log(input);
};

let load_new_albums = () => {
  if (albums.value.length < 100000) {
    albums.value.push({
      url: "https://www.bilibili.com/video/BV1zP4y1H7hZ",
      cover_url:
        "https://i2.hdslb.com/bfs/archive/6549df6d11cde68937433884ba1fd9437ff38fee.jpg",
      title: "aaaaa",
    });
    // console.log(albums.value);
  }
};
onMounted(() => {
  albums.value = [
    {
      url: "https://www.bilibili.com/video/BV1zP4y1H7hZ",
      cover_url:
        "https://i2.hdslb.com/bfs/archive/6549df6d11cde68937433884ba1fd9437ff38fee.jpg",
      title: "aaaaa",
    },
    {
      url: "https://www.bilibili.com/video/BV1zP4y1H7hZ",
      cover_url:
        "https://i2.hdslb.com/bfs/archive/6549df6d11cde68937433884ba1fd9437ff38fee.jpg",
      title: "aaaaa",
    },
  ];
});
</script>

<template>
  <el-row type="flex" justify="center" class="av-el-row">
    <el-input class="av-el-input" v-model="input" placeholder="输入关键字"></el-input>
    <el-button class="button" type="primary" @click="search" size="mini" :icon="Search" circle></el-button>
  </el-row>
  <ul v-infinite-scroll="load_new_albums" class="ac-list">
    <li v-for="a in albums" :key="a" class="ac-list-item">
      <album-card :url="a.url" :cover_url="a.cover_url" :title="a.title"></album-card>
    </li>
  </ul>
</template>

<style scoped>
.av-el-input {
  width: 15rem;
}
.button {
  margin-left: 0.5rem;
}
.av-el-row {
  align-items: center;
}
.ac-list {
  width: 100%;
  display: flex;
  justify-content: center;
  flex-wrap: wrap;
  padding: 0px;
  list-style-type: none;
  list-style-position: initial;
  list-style-image: initial;
}
</style>
