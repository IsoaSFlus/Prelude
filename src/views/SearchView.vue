<script lang="ts">
export default {
	name: "SearchView",
};
</script>
<script setup lang="ts">
import { Search, Setting } from "@element-plus/icons-vue";
import AlbumCard from "../components/AlbumCard.vue";
import SettingView from "../views/SettingView.vue";
import { ref, onMounted } from "vue";
import { invoke } from "@tauri-apps/api";

interface Album {
	id: string;
	cover_url: string;
	title: string;
}

const input = ref("");
const albums = ref(new Array<Album>());
const settings_visible = ref(false);

let search = async () => {
	albums.value = [];
	await invoke("search_spotify", {
		keywords: input.value,
	});
	await load_new_albums();
};

let open_settings = () => {
	settings_visible.value = true;
};

let load_new_albums = async () => {
	let res: any = await invoke("get_spotify_search_result");
	for (let a of res) {
		// console.log(a);
		albums.value.push({
			id: a.id,
			cover_url: a.cover_url,
			title: a.title,
		});
	}
};

onMounted(() => { });
</script>

<template>
	<el-row class="av-el-row">
		<el-col :span="12" :offset="6" class="text-input-col">
			<el-input class="av-el-input" v-model="input" @keyup.enter="search" size="large"
				placeholder="Search"></el-input>
		</el-col>
		<el-col :span="2" :offset="4" class="text-input-col">
			<el-button class="setting-button" size="large" :icon="Setting" @click="open_settings" circle>
			</el-button>
		</el-col>
	</el-row>
	<ul v-infinite-scroll="load_new_albums" class="ac-list">
		<li v-for="a in albums" :key="a.id" class="ac-list-item">
			<router-link class="sv-link"
				:to="{ name: 'sp_detail', params: { album_id: a.id, cover_url: a.cover_url, album_title: a.title }}">
				<album-card :id="a.id" :cover_url="a.cover_url" :title="a.title"></album-card>
			</router-link>
		</li>
	</ul>
	<el-dialog v-model="settings_visible" width="90%" title="Settings" destroy-on-close>
		<setting-view></setting-view>
	</el-dialog>
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

/* .setting-button { */
/* 	/* float: right; */
/* } */

/* .text-input-col {
    } */
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
