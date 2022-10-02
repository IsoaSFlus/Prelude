<script setup lang="ts">
// import { Search, Setting } from "@element-plus/icons-vue";
// import AlbumCard from "../components/AlbumCard.vue";
import { ElMessage } from "element-plus";
import { reactive, ref, onMounted } from "vue";
import { invoke } from "@tauri-apps/api";

const formRef = ref();
const form = reactive({
	bind_address: "",
	mpd_address: "",
	spotify: {
		email: "",
		password: "",
		app_id: "",
		app_secret: "",
	},
	qobuz: {
		email: "",
		password: "",
		app_id: "",
		app_secret: "",
	},
	tidal: {
		app_id: "",
		app_secret: "",
		token: "",
	},
});

let submit = async () => {
	await invoke("write_settings", { c: form });
	ElMessage({
		message: "修改成功，请重启。",
		type: "success",
	});
};

onMounted(async () => {
	let res: any = await invoke("get_settings");
	console.log(res);
	form.bind_address = res.bind_address;
	form.mpd_address = res.mpd_address;
	form.spotify = res.spotify;
	form.qobuz = res.qobuz;
	form.tidal = res.tidal;
});
</script>

<template>
	<el-form ref="formRef" :model="form" label-width="7rem">
		<h2>地址设置</h2>
		<el-form-item label="bind address">
			<el-input v-model="form.bind_address"></el-input>
		</el-form-item>
		<el-form-item label="mpd address">
			<el-input v-model="form.mpd_address"></el-input>
		</el-form-item>
		<h2>Spotify</h2>
		<el-form-item label="email">
			<el-input v-model="form.spotify.email"></el-input>
		</el-form-item>
		<el-form-item label="password">
			<el-input v-model="form.spotify.password"></el-input>
		</el-form-item>
		<el-form-item label="app_id">
			<el-input v-model="form.spotify.app_id"></el-input>
		</el-form-item>
		<el-form-item label="app_secret">
			<el-input v-model="form.spotify.app_secret"></el-input>
		</el-form-item>
		<h2>Qobuz</h2>
		<el-form-item label="email">
			<el-input v-model="form.qobuz.email"></el-input>
		</el-form-item>
		<el-form-item label="password">
			<el-input v-model="form.qobuz.password"></el-input>
		</el-form-item>
		<el-form-item label="app_id">
			<el-input v-model="form.qobuz.app_id"></el-input>
		</el-form-item>
		<el-form-item label="app_secret">
			<el-input v-model="form.qobuz.app_secret"></el-input>
		</el-form-item>
		<h2>Tidal</h2>
		<el-form-item label="app_id">
			<el-input v-model="form.tidal.app_id"></el-input>
		</el-form-item>
		<el-form-item label="app_secret">
			<el-input v-model="form.tidal.app_secret"></el-input>
		</el-form-item>
		<el-form-item label="token">
			<el-input v-model="form.tidal.token"></el-input>
		</el-form-item>
		<div>
			<el-button type="primary" @click="submit">Submit</el-button>
		</div>
	</el-form>
</template>

<style scoped>

</style>
