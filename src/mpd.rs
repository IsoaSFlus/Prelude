use crate::Album;
use anyhow::Result;
use tokio::{
    io::{AsyncBufReadExt, AsyncWriteExt},
    net::TcpStream,
};

pub struct MPD {
    address: String,
    prelude_address: String,
}

impl MPD {
    pub fn new(addr: &str, paddr: &str) -> Self {
        Self {
            address: addr.into(),
            prelude_address: paddr.into(),
        }
    }

    pub async fn add_album_to_mpd(&self, album: &Album, tp: &str) -> Result<()> {
        let socket = TcpStream::connect(&self.address).await?;
        let (readhalf, mut writehalf) = tokio::io::split(socket);
        let mut reader = tokio::io::BufReader::new(readhalf).lines();
        for t in album.tracks.iter() {
            writehalf
                .write(
                    format!(
                        "addid \"http://{}/track/{}?t={}\"\n",
                        &self.prelude_address, t.id, tp
                    )
                    .as_bytes(),
                )
                .await?;
            while let Some(line) = reader.next_line().await.unwrap() {
                println!("{}", &line);
                if line.starts_with("OK MPD") {
                    continue;
                }
                if line.eq("OK") {
                    continue;
                }
                let re = regex::Regex::new(r#"Id: ([0-9]+)"#).unwrap();
                match re.captures(&line) {
                    Some(it) => {
                        writehalf
                            .write(
                                format!(
                                    r#"command_list_begin
addtagid {0} title "{1}"
addtagid {0} album "{2}"
addtagid {0} artist "{3}"
command_list_end
"#,
                                    &it[1],
                                    &t.title.replace(r#"""#, ""),
                                    &album.title.replace(r#"""#, ""),
                                    &t.performers.replace(r#"""#, "")
                                )
                                .as_bytes(),
                            )
                            .await?;
                        break;
                    }
                    None => {
                        continue;
                    }
                };
            }
        }
        Ok(())
    }
}
