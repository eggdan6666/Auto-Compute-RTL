#!/usr/bin/env bash
# 在 SCNet 实例上下载 OSS CAD Suite (GitHub 直连不通, 依次尝试镜像)
set -u
cd /root/tools
REL="YosysHQ/oss-cad-suite-build/releases/download/2026-10-09/oss-cad-suite-linux-x64-20261009.tgz"
for M in "https://ghfast.top/https://github.com" "https://ghproxy.net/https://github.com" "https://gh-proxy.com/https://github.com" "https://github.com"; do
    echo "== trying $M =="
    # 先测速: 取前 2MB
    if timeout 30 curl -sL -o /dev/null -w "speed=%{speed_download} B/s http=%{http_code}\n" --max-time 25 -r 0-2000000 "$M/$REL"; then
        echo "== full download via $M =="
        if curl -sL --retry 3 -o oss-cad-suite.tgz "$M/$REL"; then
            echo "DOWNLOAD_OK $(ls -la oss-cad-suite.tgz)"
            exit 0
        fi
    fi
done
echo "ALL_MIRRORS_FAILED"
exit 1
