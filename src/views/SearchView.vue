<script>
export default {
  name: "SearchView",
};
</script>
<script setup>
import { Search } from "@element-plus/icons-vue";
import AlbumCard from "../components/AlbumCard.vue";
import { ref, onMounted } from "vue";
import { invoke } from "@tauri-apps/api";
const input = ref("");
const albums = ref([]);

let search = () => {
  albums.value = [];
  invoke("search_spotify", {
    keywords: input.value,
  }).then(() => {
    load_new_albums();
  });
};

let load_new_albums = () => {
  invoke("get_spotify_search_result").then((res) => {
    for (var i in res) {
      // console.log(a);
      albums.value.push({
        id: res[i].id,
        cover_url: res[i].cover_url,
        title: res[i].title,
      });
    }
  });
};

onMounted(() => {});
</script>

<template>
  <el-row type="flex" justify="center" class="av-el-row">
    <el-input class="av-el-input" v-model="input" placeholder="Search"></el-input>
    <el-button class="button" type="primary" @click="search" size="mini" :icon="Search" circle></el-button>
  </el-row>
  <ul v-infinite-scroll="load_new_albums" class="ac-list">
    <li v-for="a in albums" :key="a" class="ac-list-item">
      <router-link
        class="sv-link"
        :to="{ name: 'sp_detail', params: { id: a.id, cover_url: a.cover_url, title: a.title }}"
      >
        <album-card :id="a.id" :cover_url="a.cover_url" :title="a.title"></album-card>
      </router-link>
    </li>
  </ul>
</template>

<style scoped>
.sv-link {
  text-decoration: none;
}
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
