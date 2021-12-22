import { createRouter, createWebHistory } from 'vue-router'
import SearchView from "./views/SearchView.vue";
import AlbumDetailView from "./views/AlbumDetailView.vue";

const routes = [
    {
        path: '/',
        component: SearchView,
    },
    {
        path: '/album',
        component: AlbumDetailView,
    }
]

const router = new createRouter({
    history: createWebHistory(),
    scrollBehavior(to, from, savedPosition) {
        if (savedPosition) {
            return savedPosition
        } else {
            return { x: 0, y: 0 }
        }
    },
    routes,
})

export {
    router,
}
