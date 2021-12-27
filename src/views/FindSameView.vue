<script setup>
import AlbumCard from "../components/AlbumCard.vue";
import { ref, onMounted } from "vue";
import { invoke } from "@tauri-apps/api";
const input = ref("");
const albums = ref([]);

let add_to_mpd = (id) => {
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
  <el-row type="flex" justify="center" class="av-el-row"></el-row>
  <ul v-infinite-scroll="load_new_albums" class="ac-list">
    <li v-for="a in albums" :key="a" class="ac-list-item">
      <album-card :id="a.id" :cover_url="a.cover_url" :title="a.title" @click="add_to_mpd(a.id)"></album-card>
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
