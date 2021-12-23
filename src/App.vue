<script setup>
// This starter template is using Vue 3 <script setup> SFCs
// Check out https://v3.vuejs.org/api/sfc-script-setup.html#sfc-script-setup
import { invoke } from "@tauri-apps/api";
import { ref, onMounted } from "vue";

onMounted(() => {
  invoke("init")
    .then(() => {})
    .catch((e) => {
      console.error(e);
    });
});
</script>

<template>
  <div>
    <div id="main">
      <router-view v-slot="{ Component }">
        <transition>
          <keep-alive include="SearchView">
            <component :is="Component" />
          </keep-alive>
        </transition>
      </router-view>
    </div>
  </div>
</template>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  /* margin-top: 60px; */
  margin: 0 auto;
}
</style>
