<script setup lang="ts">
import { Track } from "./interface";

const props = defineProps({
	tracks: Array<Track>,
});
</script>

<template>
	<div class="container">
		<el-row type="flex" justify="center" class="song-list-header">
			<el-col :span="1" class="col-left">
				<span>#</span>
			</el-col>
			<el-col :span="22" class="col-left">
				<span>标题</span>
			</el-col>
			<el-col :span="1" class="col-right">
				<span>时长</span>
			</el-col>
		</el-row>
		<template v-for="(t, i) in props.tracks" :key="t.id">
			<el-row type="flex" justify="center" class="song-item">
				<el-col :span="1" class="col-left">
					<span class="index">{{ i + 1 }}</span>
				</el-col>
				<el-col :span="22" class="col-left">
					<span>{{t.title}}</span>
				</el-col>
				<el-col :span="1" class="col-right">
					<span class="duration">{{ t.duration }}</span>
				</el-col>
			</el-row>
		</template>
	</div>
</template>

<style lang="less" scoped>
@global-width: 90%;

.container {
	margin-top: 2rem;
	margin-bottom: 2rem;
}

.song-list-header {
	width: @global-width;
	margin: auto;
}

.song-item {
	width: @global-width;
	margin: auto;
}

.col-left {
	text-align: left;
}

.col-right {
	text-align: right;
}

.duration {
	font-family: Courier, monospace;
}

.index {
	.duration
}
</style>
