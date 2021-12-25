<script setup>
import { ref, onMounted } from "vue";
import { useRouter, useRoute } from "vue-router";
import { invoke } from "@tauri-apps/api";
import { Back } from "@element-plus/icons-vue";

const tracks = ref([]);
const router = useRouter();
const route = useRoute();

let go_back = () => {
  router.back();
};

let add_to_mpd = () => {
  let a = [];
  for (var i in tracks.value) {
    a.push({
      id: tracks.value[i].id,
      title: tracks.value[i].title,
      performers: tracks.value[i].performers,
    });
  }
  invoke("add_spotify_tracks_to_mpd", {
    album: {
      img: route.params.cover_url,
      title: route.params.title,
      tracks: a,
    },
  }).then(() => {});
};
// const props = defineProps({});
let secToMMSS = (sec) => {
  let m = Math.floor(sec / 60);
  let s = sec % 60;
  if (s < 10) {
    s = "0" + s;
  }
  if (m < 10) {
    m = "0" + m;
  }
  return m + ":" + s;
};
onMounted(() => {
  invoke("get_spotify_album_tracks", {
    id: route.params.id,
  })
    .then((res) => {
      for (var i in res) {
        // console.log(a);
        tracks.value.push({
          id: res[i].id,
          duration: secToMMSS(res[i].duration),
          title: res[i].title,
          performers: res[i].performers,
        });
      }
    })
    .catch((e) => {
      console.error(e);
    });
});
</script>

<template>
  <el-row class="adv-row" type="flex" justify="center">
    <el-col :span="12">
      <el-button class="button" type="primary" @click="go_back" size="mini" :icon="Back" circle></el-button>
      <div>
        <img :src="route.params.cover_url" class="image" />
      </div>
      <span>{{route.params.title}}</span>
      <el-button class="button" type="primary" @click="add_to_mpd" size="mini" :icon="Back" circle></el-button>
    </el-col>
    <el-col :span="12">
      <el-table :data="tracks" style="width: 100%">
        <el-table-column prop="title" label="标题" width="400" />
        <el-table-column prop="duration" label="时间" width="70" />
      </el-table>
    </el-col>
  </el-row>
</template>

<style scoped>
.adv-row {
  justify-items: center;
}
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
  width: 25rem;
  height: 25rem;
  object-fit: cover;
  /* display: block; */
}
.card-video-title {
  max-width: 15.5rem;
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
