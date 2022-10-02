<script lang="ts">
export default {
	name: "AlbumDetailView",
};
</script>
<script setup lang="ts">
import { ref, onMounted, Ref } from "vue";
import { useRouter, useRoute } from "vue-router";
import { invoke } from "@tauri-apps/api";
import FindSameView from "./FindSameView.vue";
import { Back } from "@element-plus/icons-vue";

interface Track {
	id: string;
	duration: string;
	title: string;
	performers: string;
}

const tracks: Ref<Array<Track>> = ref([]);
const find_same_visible = ref(false);
const find_same_site = ref("");
const router = useRouter();
const route = useRoute();

let go_back = () => {
	router.back();
};

let find_same = (site: string) => {
	find_same_site.value = site;
	find_same_visible.value = true;
};

let add_to_mpd = async () => {
	let a = [];
	for (let t of tracks.value) {
		a.push({
			id: t.id,
			title: t.title,
			performers: t.performers,
		});
	}
	await invoke("add_spotify_tracks_to_mpd", {
		album: {
			img: route.params.cover_url as string,
			title: route.params.album_title as string,
			tracks: a,
		},
	});
};

let secToMMSS = (sec: number) => {
	let m = Math.floor(sec / 60);
	let s = sec % 60;
	// if (s < 10) {
	// 	sstr = "0" + s.toString();
	// }
	// if (m < 10) {
	// 	m = "0" + m;
	// }
	return ((m < 10) ? "0" + m.toString() : m.toString()) + ":" + ((s < 10) ? "0" + s.toString() : s.toString());
};
onMounted(async () => {
	// console.log(props.album_id);
	try {
		let res: any = await invoke("get_spotify_album_tracks", {
			id: route.params.album_id,
		});
		for (let t of res) {
			console.log(t);
			tracks.value.push({
				id: t.id,
				duration: secToMMSS(t.duration),
				title: t.title,
				performers: t.performers,
			});
		}
	} catch (e) {
		console.error(e);
	}
});
</script>

<template>
	<el-row class="adv-row" type="flex" justify="center">
		<el-col :span="24">
			<el-row class="adv-back-button-row" type="flex" justify="left">
				<el-button class="adv-back-button" type="primary" @click="go_back" size="large"
					:icon="Back" circle></el-button>
			</el-row>
			<div>
				<img :src="route.params.cover_url as string" class="cover-image" />
			</div>
			<div class="adv-album-title">
			<span >{{route.params.album_title}}</span>
			</div>
			<el-row class="adv-add-button-row" type="flex" justify="center">
				<el-button class="adv-add-button" type="primary" @click="add_to_mpd" size="medium">Add
					to MPD</el-button>
				<el-button class="adv-add-button" type="primary" @click="find_same('tidal')"
					size="medium">Find in Tidal</el-button>
				<el-button class="adv-add-button" type="primary" @click="find_same('qobuz')"
					size="medium">Find in Qobuz</el-button>
			</el-row>
			<el-table :data="tracks">
				<el-table-column prop="title" label="标题" width="600" />
				<el-table-column prop="duration" label="时间" align="right" width="100" />
			</el-table>
		</el-col>
	</el-row>
	<el-dialog v-model="find_same_visible" width="70%" title="Find the same album and add to mpd" destroy-on-close>
		<find-same-view :site="find_same_site" :keywords="route.params.album_title as string"></find-same-view>
	</el-dialog>
</template>

<style scoped>
.adv-row {
	justify-items: center;
}

.adv-back-button {
	margin-left: 0.5rem;
	margin-top: 0.5rem;
}

.adv-add-button {
	margin-left: 0.5rem;
	margin-right: 0.5rem;
}

.adv-album-title{
	margin-top: 1.5rem;
	margin-bottom: 1.5rem;
	font-size: larger;
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

.cover-image {
	margin-top: 1rem;
	width: 35rem;
	height: 35rem;
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
