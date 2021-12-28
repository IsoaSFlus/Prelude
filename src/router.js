import { createRouter, createWebHashHistory } from 'vue-router'
import SearchView from "./views/SearchView.vue";
import AlbumDetailView from "./views/AlbumDetailView.vue";

const routes = [
    {
        path: '/',
        name: 'appmain',
        component: SearchView,
    },
    {
        path: '/album',
        name: 'sp_detail',
        component: AlbumDetailView,
    },
]

const router = new createRouter({
    history: createWebHashHistory(),
    // scrollBehavior(to, from, savedPosition) {
    //     if (savedPosition) {
    //         return savedPosition
    //     } else {
    //         return { x: 0, y: 0 }
    //     }
    // },
    routes,
})

export {
    router,
}
