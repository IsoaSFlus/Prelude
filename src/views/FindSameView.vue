<script setup lang="ts">
import AlbumCard from "../components/AlbumCard.vue";
// import { Back } from "@element-plus/icons-vue";
import { ElMessage } from "element-plus";
import { ref, onMounted, Ref } from "vue";
import { invoke } from "@tauri-apps/api";

interface Album {
	id: string;
	cover_url: string;
	title: string;
	hires: boolean;
}

const albums: Ref<Array<Album>> = ref([]);
const props = defineProps({
	site: String,
	keywords: String,
});

let add_to_mpd = async (id: string) => {
	try {
		await invoke("add_album_to_mpd", {
			id: id,
			tp: props.site,
		});
		ElMessage({
			message: "添加成功！",
			type: "success",
		});
	} catch (e) {
		ElMessage.error("添加失败: " + e);
	}
};

let load_new_albums = () => { };

onMounted(async () => {
	if (props.site == "tidal") {
		let res: any = await invoke("search_in_tidal", {
			keywords: props.keywords,
		});
		albums.value = res;
	} else {
		let res: any = await invoke("search_in_qobuz", {
			keywords: props.keywords,
		});
		albums.value = res;
	}
});
</script>

<template>
	<!-- <el-row class="back-button-row" type="flex" justify="left"></el-row> -->
	<ul v-infinite-scroll="load_new_albums" class="ac-list">
		<li v-for="a in albums" :key="a.id" class="ac-list-item">
			<album-card :id="a.id" :hires="a.hires" :cover_url="a.cover_url" :title="a.title"
				@click="add_to_mpd(a.id)"></album-card>
		</li>
	</ul>
</template>

<style scoped>
.av-el-input {
	width: 15rem;
}

.back-button {
	margin-left: 0.5rem;
	margin-top: 0.5rem;
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
